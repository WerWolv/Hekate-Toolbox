#include "titleinfo.hpp"
#include "jpeg.hpp"
#include <cstring>
#include <thread>
#include <condition_variable>
#include <memory>

struct JpegWorkObject {
  std::shared_ptr<TitleIcon> title;
  WidthHeight res;
};

static std::vector<JpegWorkObject> workObjects{};
static std::condition_variable workEvent;
static std::mutex m;

static bool keepRunning = true;
static std::thread jpegThread;


static void jpegDecThreadLoop() {
  workObjects.reserve(512);

  auto appControlData = std::make_unique<NsApplicationControlData>();
  size_t appControlDataSize = 0;
  NacpLanguageEntry *languageEntry = nullptr;
  Result rc;

  std::unique_lock<std::mutex> lk(m);
  while (keepRunning) {

    workEvent.wait(lk);

    for (auto it = workObjects.begin(); it != workObjects.end(); ++it) {

      //Get the big application data
      rc = nsGetApplicationControlData(NsApplicationControlSource_Storage, it->title->application_id, appControlData.get(), sizeof(NsApplicationControlData), &appControlDataSize);
      if (R_SUCCEEDED(rc)) {

        //Get the title name
        rc = nacpGetLanguageEntry(&appControlData->nacp, &languageEntry);
        if (R_SUCCEEDED(rc)) {
          strcpy(it->title->name, languageEntry->name);
        }

        //Decompress the jpeg
        const u8 *img = jpegdec(appControlData->icon, sizeof(appControlData->icon), it->res);
        it->title->icon = std::unique_ptr<const u8>(img);
      }

      //erase the processed object from the vector
      workObjects.erase(it--);
    }
  }
}


void initJpegThread() {
  keepRunning = true;
  jpegThread = std::thread(jpegDecThreadLoop);
}

void exitJpegThread() {
  keepRunning = false;
  workEvent.notify_one();
  if (jpegThread.joinable())
    jpegThread.join();
}

///
/// END THREAD STUFF
///

std::shared_ptr<TitleIcon> DumpTitle(u64 appID, WidthHeight res) {

  auto appData = std::make_shared<TitleIcon>();
  
  //Get the title id
  appData->application_id = appID;

  //pass the control data object to our work buffer that will then process the jpeg image independently
  appData->icon = nullptr;
  workObjects.push_back(JpegWorkObject{appData, res});
  workEvent.notify_one();
  return appData;
}

std::vector<u64> DumpAllAppIDs() {
  auto appRecords = std::vector<NsApplicationRecord>(512);
  auto titleIDs = std::vector<u64>();
  titleIDs.reserve(512);
  s32 actualAppRecordCnt = 0;
  Result rc = nsListApplicationRecord(appRecords.data(), sizeof(NsApplicationRecord) * 512, 0, &actualAppRecordCnt);
  if (R_SUCCEEDED(rc)) {
    for (s32 i=0; i != actualAppRecordCnt; ++i) {
      titleIDs.push_back(appRecords[i].application_id);
    }
  }
  return titleIDs;
}

std::vector<std::shared_ptr<TitleIcon>> DumpAllTitles(WidthHeight res) {
  auto appControlDatas = std::vector<std::shared_ptr<TitleIcon>>();
  appControlDatas.reserve(512);

  auto titleIDs = DumpAllAppIDs();
  
  for (auto &&titleID : titleIDs)
  {
    appControlDatas.push_back(std::move(DumpTitle(titleID, res)));
  }
  return appControlDatas;
}

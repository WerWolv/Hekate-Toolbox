#include "gametitle.hpp"
#include "jpeg.hpp"
#include <cstring>

std::shared_ptr<TitleIcon> DumpGame(u64 appID, WidthHeight res) {

  auto appData = std::make_shared<TitleIcon>();

  NsApplicationControlData appControlData;
  size_t appControlDataSize = 0;
  NacpLanguageEntry *languageEntry = nullptr;
  
  //Get the title id
  appData->application_id = appID;

  //Get the bitmap image of the icon
  Result rc = nsGetApplicationControlData(NsApplicationControlSource_Storage, appID, &appControlData, sizeof(NsApplicationControlData), &appControlDataSize);
  if (R_SUCCEEDED(rc)) {
    const u8 *img = jpegdec(appControlData.icon, sizeof(appControlData.icon), res);
    appData->icon = std::unique_ptr<const u8>(img);

    //Get the title name
    rc = nacpGetLanguageEntry(&appControlData.nacp, &languageEntry);
    if (R_SUCCEEDED(rc)) {
      strcpy(appData->name, languageEntry->name);
    }
  }
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

std::vector<std::shared_ptr<TitleIcon>> DumpAllGames(WidthHeight res) {
  auto appControlDatas = std::vector<std::shared_ptr<TitleIcon>>();
  appControlDatas.reserve(512);

  auto titleIDs = DumpAllAppIDs();
  
  for (auto &&titleID : titleIDs)
  {
    appControlDatas.push_back(std::move(DumpGame(titleID, res)));
  }
  return appControlDatas;
}

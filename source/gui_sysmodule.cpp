#include "gui_sysmodule.hpp"
#include "button.hpp"

#include <stdio.h>
#include <dirent.h>
#include <sstream>
#include <fstream>
#include <utility>

#include "list_selector.hpp"

#include "json.hpp"

using json = nlohmann::json;

extern "C" {
  #include "pm_dmnt.h"
}

static u64 cur_val, lim_val;

GuiSysmodule::GuiSysmodule() : Gui() {
  pmshellInitialize();
  pmdmntInitialize();
  pmdmntInitialize_mod();

  pmdmntGetCurrentLimitInfo(&cur_val, &lim_val, 0, 0);

  std::ifstream configFile("/switch/KosmosToolbox/config.json");

  if (configFile.fail()) {
    Gui::g_nextGui = GUI_MAIN;
    printf("Failed to open config file!\n");
    return;
  }

  json configJson;
  try {
    configFile >> configJson;
  } catch(json::parse_error& e) {
    printf("Failed to parse JSON file!\n");
    return;
  }

  for (auto sysmodule : configJson["sysmodules"]) {
    try {
      this->m_sysmodules.insert(std::make_pair(sysmodule["tid"].get<std::string>(), (sysModule_t){ sysmodule["name"], sysmodule["desc"], sysmodule["tid"], sysmodule["maxRAM"]}));
    
      u64 sysmodulePid = 0;
      pmdmntGetTitlePid(&sysmodulePid, std::stoul(sysmodule["tid"].get<std::string>(), nullptr, 16));

      if (sysmodulePid > 0)
        this->m_runningSysmodules.insert(sysmodule["tid"].get<std::string>());
    } catch(json::parse_error &e) {
      continue;
    }
  }

  printf("\n");


  u16 xOffset = 0, yOffset = 0;
  s32 cnt = 0;
  u32 sysmoduleCnt = this->m_sysmodules.size();

  for (auto &sysmodule : this->m_sysmodules) {
     new Button(100 + xOffset, 200 + yOffset, 500, 80, [&](Gui *gui, u16 x, u16 y, bool *isActivated){
      gui->drawTextAligned(font20, x + 37, y + 50, currTheme.textColor, sysmodule.second.name.c_str(), ALIGNED_LEFT);
      gui->drawTextAligned(font20, x + 420, y + 50, this->m_runningSysmodules.find(sysmodule.first) != this->m_runningSysmodules.end() ? currTheme.selectedColor : Gui::makeColor(0xB8, 0xBB, 0xC2, 0xFF), this->m_runningSysmodules.find(sysmodule.first) != this->m_runningSysmodules.end() ? "On" : "Off", ALIGNED_LEFT);
    }, [&](u32 kdown, bool *isActivated){
      if (kdown & KEY_A) {
        u64 pid;
        u64 tid = std::stol(sysmodule.first.c_str(), nullptr, 16);

        std::stringstream path;
        path << "/atmosphere/titles/" << sysmodule.first << "/flags/boot2.flag";


        if (this->m_runningSysmodules.find(sysmodule.first) != this->m_runningSysmodules.end()) {
          pmshellTerminateProcessByTitleId(tid);
          remove(path.str().c_str());
        }
        else {
          if (R_SUCCEEDED(pmshellLaunchProcess(0, tid, FsStorageId_None, &pid))) {
            FILE *fptr = fopen(path.str().c_str(), "wb+");
            if (fptr != nullptr) fclose(fptr);
          }
        }

        pid = 0;
        pmdmntGetTitlePid(&pid, tid);

        if (pid != 0)
          this->m_runningSysmodules.insert(sysmodule.first);
        else
          this->m_runningSysmodules.erase(sysmodule.first);   

        pmdmntGetCurrentLimitInfo(&cur_val, &lim_val, 0, 0);      
      }
    }, { (cnt - 1), (u32)(cnt + 1) == sysmoduleCnt ? -1 : (cnt + 1), (cnt - 4) >= 0 ? cnt - 4 : -1, (u32)(cnt + 4) < sysmoduleCnt ? cnt + 4 : -1 }, false, 
    [&]() -> bool { return this->m_runningSysmodules.find(sysmodule.first) != this->m_runningSysmodules.end() || (lim_val - cur_val) > (sysmodule.second.maxRAM * 1024); });
  

    yOffset += 100;

    if (yOffset == 400) {
      xOffset += 550;
      yOffset = 0;
    }

    cnt++;
  }

}

GuiSysmodule::~GuiSysmodule() {
  pmshellExit();
  pmdmntExit();
  pmdmntExit_mod();

  Button::g_buttons.clear();
}

void GuiSysmodule::update() {
  Gui::update();
}

void GuiSysmodule::draw() {
  Gui::beginDraw();
  Gui::drawRectangle(0, 0, Gui::g_framebuffer_width, Gui::g_framebuffer_height, currTheme.backgroundColor);
  Gui::drawRectangle((u32)((Gui::g_framebuffer_width - 1220) / 2), 87, 1220, 1, currTheme.textColor);
  Gui::drawRectangle((u32)((Gui::g_framebuffer_width - 1220) / 2), Gui::g_framebuffer_height - 73, 1220, 1, currTheme.textColor);
  Gui::drawTextAligned(fontIcons, 70, 68, currTheme.textColor, "\uE130", ALIGNED_LEFT);
  Gui::drawTextAligned(font24, 70, 58, currTheme.textColor, "        Kosmos Toolbox", ALIGNED_LEFT);
  Gui::drawTextAligned(font20, Gui::g_framebuffer_width - 50, Gui::g_framebuffer_height - 25, currTheme.textColor, "\uE0E1 Back     \uE0E0 Ok", ALIGNED_RIGHT);

  for(Button *btn : Button::g_buttons)
    btn->draw(this);
  Gui::endDraw();
}

void GuiSysmodule::onInput(u32 kdown) {
  for(Button *btn : Button::g_buttons) {
    if (btn->isSelected())
      if (btn->onInput(kdown)) break;
  }
}

void GuiSysmodule::onTouch(touchPosition &touch) {
  for(Button *btn : Button::g_buttons) {
    btn->onTouch(touch);
  }
}

void GuiSysmodule::onGesture(touchPosition &startPosition, touchPosition &endPosition) {

}

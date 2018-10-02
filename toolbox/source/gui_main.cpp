#include "gui_main.hpp"
#include "button.hpp"

#include <stdio.h>
#include <dirent.h>

#include "list_selector.hpp"

extern "C" {
  #include "ldr_cfg.h"
}

static std::vector<std::string> autobootNames;
static u16 currAutoBootEntryIndex;

GuiMain::GuiMain() : Gui() {
  m_loaderIni = new mINI::INIFile("sdmc:/atmosphere/loader.ini");
  m_loaderIni->read(m_ini);

  ldrCfgInitialize();

  getCurrOverrideKeyCombo(&m_overrideKeyCombo);
  getOverrideByDefault(&m_overrideByDefault);
  m_currAutoBootConfig = getAutoBootConfigs(m_autoBootConfigs, currAutoBootEntryIndex);

  new Button(150, 240, 200, 200, [&](Gui *gui, u16 x, u16 y, bool *isActivated){
    gui->drawTextAligned(fontHuge, x + 37, y + 145, currTheme.textColor, keyToUnicode(m_overrideKeyCombo), ALIGNED_LEFT);
    gui->drawTextAligned(font14, x + 100, y + 185, currTheme.textColor, "HBMenu key combo", ALIGNED_CENTER);
  }, [&](u32 kdown, bool *isActivated){
    if(*isActivated) {
      if(!(kdown & (kdown - 1)) && (kdown <= KEY_DDOWN || kdown >= KEY_SL) && kdown != KEY_TOUCH) {
        setCurrOverrideKeyCombo(kdown, &m_overrideKeyCombo);
        m_ini["config"]["override_key"] = GuiMain::keyToKeyChars(m_overrideKeyCombo, m_overrideByDefault);
        m_loaderIni->write(m_ini, true);
        *isActivated = false;
      }
    }
  }, { -1, -1, -1, 1 }, true);

   new Button(370, 240, 700, 80, [&](Gui *gui, u16 x, u16 y, bool *isActivated){
     gui->drawTextAligned(font20, x + 37, y + 50, currTheme.textColor, "Open \uE134 by default", ALIGNED_LEFT);
     gui->drawTextAligned(font20, x + 620, y + 50, !m_overrideByDefault ? currTheme.selectedColor : Gui::makeColor(0xB8, 0xBB, 0xC2, 0xFF), !m_overrideByDefault ? "On" : "Off", ALIGNED_LEFT);
   }, [&](u32 kdown, bool *isActivated){
     if (kdown & KEY_A) {
        setOverrideByDefault(!m_overrideByDefault, &m_overrideByDefault);
        m_ini["config"]["override_key"] = GuiMain::keyToKeyChars(m_overrideKeyCombo, m_overrideByDefault);
        m_loaderIni->write(m_ini, true);
      }
   }, { -1, 2, 0, -1 }, false);

   new Button(370, 340, 700, 80, [&](Gui *gui, u16 x, u16 y, bool *isActivated){
     gui->drawTextAligned(font20, x + 37, y + 50, currTheme.textColor, "Hekate autoboot", ALIGNED_LEFT);

     std::string autoBootName = m_currAutoBootConfig.name;

     if(autoBootName.length() >= 25) {
       autoBootName = autoBootName.substr(0, 24);
       autoBootName += "...";
     }

     gui->drawTextAligned(font20, x + 660, y + 50, currTheme.selectedColor, autoBootName.c_str(), ALIGNED_RIGHT);
   }, [&](u32 kdown, bool *isActivated){
     if (kdown & KEY_A) {
       autobootNames.clear();

       for(auto const& autoBootEntry : m_autoBootConfigs)
         autobootNames.push_back(autoBootEntry.name);

       (new ListSelector("Hekate autoboot", "\uE0E1 Back     \uE0E0 Ok", autobootNames, currAutoBootEntryIndex))->setInputAction([&](u32 k, u16 selectedItem){
         if(k & KEY_A) {
           mINI::INIFile hekateIni("sdmc:/bootloader/hekate_ipl.ini");
           mINI::INIStructure ini;

           hekateIni.read(ini);
           currAutoBootEntryIndex = selectedItem;
           m_currAutoBootConfig = m_autoBootConfigs[selectedItem];
           ini["config"]["autoboot"] = std::to_string(m_currAutoBootConfig.id);
           ini["config"]["autoboot_list"] = std::to_string(m_currAutoBootConfig.autoBootList);
           hekateIni.write(ini, true);
           Gui::g_currListSelector->hide();
         }
       })->show();
     }
   }, { 1, -1, 0, -1 }, false);
}

GuiMain::~GuiMain() {
  ldrCfgExit();

  delete m_loaderIni;
}

const char* GuiMain::keyToUnicode(u64 key) {
  switch(key) {
    case KEY_A:       return "\uE0E0";
    case KEY_B:       return "\uE0E1";
    case KEY_X:       return "\uE0E2";
    case KEY_Y:       return "\uE0E3";
    case KEY_LSTICK:  return "\uE101";
    case KEY_RSTICK:  return "\uE102";
    case KEY_L:       return "\uE0E4";
    case KEY_R:       return "\uE0E5";
    case KEY_ZL:      return "\uE0E6";
    case KEY_ZR:      return "\uE0E7";
    case KEY_PLUS:    return "\uE0EF";
    case KEY_MINUS:   return "\uE0F0";
    case KEY_DLEFT:   return "\uE0ED";
    case KEY_DUP:     return "\uE0EB";
    case KEY_DRIGHT:  return "\uE0EE";
    case KEY_DDOWN:   return "\uE0EC";
    case KEY_SL:      return "\uE0E8";
    case KEY_SR:      return "\uE0E9";
    default:          return "\uE152";
  }
}

std::string GuiMain::keyToKeyChars(u64 key, bool overrideByDefault) {
  std::string ret = overrideByDefault ? "!" : "";
  switch(key) {
    case KEY_A:       ret += "A";        break;
    case KEY_B:       ret += "B";        break;
    case KEY_X:       ret += "X";        break;
    case KEY_Y:       ret += "Y";        break;
    case KEY_LSTICK:  ret += "LS";       break;
    case KEY_RSTICK:  ret += "RS";       break;
    case KEY_L:       ret += "L";        break;
    case KEY_R:       ret += "R";        break;
    case KEY_ZL:      ret += "ZL";       break;
    case KEY_ZR:      ret += "ZR";       break;
    case KEY_PLUS:    ret += "PLUS";     break;
    case KEY_MINUS:   ret += "MINUS";    break;
    case KEY_DLEFT:   ret += "DLEFT";    break;
    case KEY_DUP:     ret += "DUP";      break;
    case KEY_DRIGHT:  ret += "DRIGHT";   break;
    case KEY_DDOWN:   ret += "DDOWN";    break;
    case KEY_SL:      ret += "SL";       break;
    case KEY_SR:      ret += "SR";       break;
  }

  return ret;
}

AutoBootEntry GuiMain::getAutoBootConfigs(std::vector<AutoBootEntry> &out_bootEntries, u16 &currAutoBootEntryIndex) {
  mINI::INIFile *hekateIni = new mINI::INIFile("sdmc:/bootloader/hekate_ipl.ini");
  mINI::INIStructure ini;
  u16 id = 0;
  AutoBootEntry currEntry;

  hekateIni->read(ini);



  u16 currAutoboot = std::stoi(ini["config"]["autoboot"], nullptr);
  bool currAutoboot_list = std::stoi(ini["config"]["autoboot_list"], nullptr);

  out_bootEntries.push_back({ "Disable autoboot", 0, false });

  for (auto const& it : ini) {
    if(it.first == "config") continue;
    out_bootEntries.push_back({ it.first, ++id, false });

    if(!currAutoboot_list && id == currAutoboot) {
      currEntry = out_bootEntries.back();
      currAutoBootEntryIndex = out_bootEntries.size() - 1;
    }
  }

  delete hekateIni;

  DIR *dr = opendir("sdmc:/bootloader/ini");
  struct dirent *de;
  std::vector<std::string> iniFiles;


  while((de = readdir(dr)) != nullptr)
    iniFiles.push_back(de->d_name);
  closedir(dr);

  std::sort(iniFiles.begin(), iniFiles.end());

  id = 0;

  for(auto const& iniFile : iniFiles) {
    hekateIni = new mINI::INIFile(std::string("sdmc:/bootloader/ini/") + iniFile);
    hekateIni->read(ini);

    for (auto const& it : ini) {
      out_bootEntries.push_back({ it.first, ++id, true });

      if(currAutoboot_list && id == currAutoboot) {
        currEntry = out_bootEntries.back();
        currAutoBootEntryIndex = out_bootEntries.size() - 1;
      }
    }

    delete hekateIni;
  }

  return currEntry;
}

void GuiMain::update() {
  Gui::update();
}

void GuiMain::draw() {
  Gui::beginDraw();

  Gui::drawRectangle(0, 0, Gui::g_framebuffer_width, Gui::g_framebuffer_height, currTheme.backgroundColor);
  Gui::drawRectangle((u32)((Gui::g_framebuffer_width - 1220) / 2), 87, 1220, 1, currTheme.textColor);
  Gui::drawRectangle((u32)((Gui::g_framebuffer_width - 1220) / 2), Gui::g_framebuffer_height - 73, 1220, 1, currTheme.textColor);

  Gui::drawTextAligned(fontIcons, 70, 68, currTheme.textColor, "\uE130", ALIGNED_LEFT);
  Gui::drawTextAligned(font24, 70, 58, currTheme.textColor, "        CFW settings", ALIGNED_LEFT);
  Gui::drawTextAligned(font20, Gui::g_framebuffer_width - 50, Gui::g_framebuffer_height - 25, currTheme.textColor, "\uE0E1 Back     \uE0E0 Ok", ALIGNED_RIGHT);
  Gui::drawTextAligned(font24, Gui::g_framebuffer_width / 2, Gui::g_framebuffer_height - 130, currTheme.textColor, "Press \uE0F4 to save and return back to the home menu", ALIGNED_CENTER);


  for(Button *btn : Button::g_buttons)
    btn->draw(this);

  Gui::endDraw();
}

void GuiMain::onInput(u32 kdown) {
  for(Button *btn : Button::g_buttons) {
    if (btn->isSelected())
      btn->onInput(kdown);
  }
}

void GuiMain::onTouch(touchPosition &touch) {
  for(Button *btn : Button::g_buttons) {
    btn->onTouch(touch);
  }
}

void GuiMain::onGesture(touchPosition &startPosition, touchPosition &endPosition) {

}

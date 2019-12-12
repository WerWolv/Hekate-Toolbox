#include "gui_main.hpp"
#include "button.hpp"

#include <sstream>

#include "utils.hpp"
#include "ini/simple_ini_parser.hpp"

#include "list_selector.hpp"
#include "override_key.hpp"

extern "C" {
  #include "pm_dmnt.h"
}

extern bool g_exitApplet;
bool exitDisabled = false;

static std::vector<std::string> autobootNames;
static u16 currAutoBootEntryIndex;

GuiMain::GuiMain() : Gui() {
  {
    // Get the override keys, if any exist
    simpleIniParser::Ini *ini = simpleIniParser::Ini::parseOrCreateFile(LOADER_INI);
    m_albumOverrideKey = OverrideKey::StringToKeyCombo(ini->findOrCreateSection(simpleIniParser::SECTION, "hbl_config")->findOrCreateFirstOption("override_key", "!R")->value);
    delete ini;
  }

  m_autoBootConfigs.push_back({ "Disable autoboot", 0, false });
  m_currAutoBootConfig = getBootConfigs(m_autoBootConfigs, currAutoBootEntryIndex);

  //0
  new Button(150, 200, 200, 200, [&](Gui *gui, u16 x, u16 y, bool *isActivated){
    gui->drawTextAligned(fontHuge, x + 37, y + 145, currTheme.textColor, OverrideKey::KeyToUnicode(m_albumOverrideKey.key), ALIGNED_LEFT);
    gui->drawTextAligned(font14, x + 100, y + 185, currTheme.textColor, "Override key combo", ALIGNED_CENTER);
  }, [&](u64 kdown, bool *isActivated){
    exitDisabled = true;
    if(*isActivated) {
      if(!(kdown & (kdown - 1)) && (kdown <= KEY_DDOWN || kdown >= KEY_SL) && kdown != KEY_TOUCH) {
        m_albumOverrideKey.key = static_cast<HidControllerKeys>(kdown);
        //Find or create a loader ini file with set override_key values, and write the result to the file.
        simpleIniParser::Ini *ini = simpleIniParser::Ini::parseOrCreateFile(LOADER_INI);
        auto keyValue = m_albumOverrideKey.ToString();
        ini->findOrCreateSection(simpleIniParser::SECTION, "hbl_config")->findOrCreateFirstOption("override_key", "")->value = keyValue;

        ini->writeToFile(LOADER_INI);
        *isActivated = false;

        delete ini;
      }
    }
    exitDisabled = false;
  }, { -1, -1, -1, 1 }, true, []() -> bool {return true;});

  //1
  new Button(370, 200, 700, 80, [&](Gui *gui, u16 x, u16 y, bool *isActivated){
     gui->drawTextAligned(font20, x + 37, y + 50, currTheme.textColor, "Override application by default", ALIGNED_LEFT);
     gui->drawTextAligned(font20, x + 620, y + 50, m_albumOverrideKey.overrideByDefault ? currTheme.selectedColor : Gui::makeColor(0xB8, 0xBB, 0xC2, 0xFF), m_albumOverrideKey.overrideByDefault ? "On" : "Off", ALIGNED_LEFT);
   }, [&](u32 kdown, bool *isActivated){
     if (kdown & KEY_A) {
        m_albumOverrideKey.overrideByDefault = !m_albumOverrideKey.overrideByDefault;
        //Find or create a loader ini file with set override_key values, and write the result to the file.
        simpleIniParser::Ini *ini = simpleIniParser::Ini::parseOrCreateFile(LOADER_INI);
        auto keyValue = m_albumOverrideKey.ToString();
        ini->findOrCreateSection(simpleIniParser::SECTION, "hbl_config")->findOrCreateFirstOption("override_key", "")->value = keyValue;

        ini->writeToFile(LOADER_INI);
        delete ini;
      }
   }, { -1, 2, 0, -1 }, false, []() -> bool {return true;});

  //2
  new Button(370, 300, 700, 80, [&](Gui *gui, u16 x, u16 y, bool *isActivated){
     gui->drawTextAligned(font20, x + 37, y + 50, currTheme.textColor, "Hekate autoboot profile", ALIGNED_LEFT);

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

       (new ListSelector("Hekate autoboot profile", "\uE0E1 Back     \uE0E0 Ok", autobootNames, currAutoBootEntryIndex))->setInputAction([&](u32 k, u16 selectedItem){
         if(k & KEY_A) {
           simpleIniParser::Ini *hekateIni = simpleIniParser::Ini::parseFile(HEKATE_INI);
           currAutoBootEntryIndex = selectedItem;
           m_currAutoBootConfig = m_autoBootConfigs[selectedItem];

           auto ini_autoboot = hekateIni->findSection("config")->findFirstOption("autoboot");
           auto ini_autoboot_list = hekateIni->findSection("config")->findFirstOption("autoboot_list");

           ini_autoboot->value = std::to_string(m_currAutoBootConfig.id);
           ini_autoboot_list->value = std::to_string(m_currAutoBootConfig.autoBootList);

           hekateIni->writeToFile(HEKATE_INI);
           Gui::g_currListSelector->hide();
         }
       })->show();
     }
   }, { 1, 3, 0, -1 }, false, []() -> bool {return true;});

  //3
  new Button(370, 400, 340, 80, [&](Gui *gui, u16 x, u16 y, bool *isActivated){
    gui->drawRectangled(x, y, 340, 80, currTheme.selectedColor);
    gui->drawTextAligned(font20, x + 37, y + 50, COLOR_BLACK, "Background services", ALIGNED_LEFT);
    gui->drawTextAligned(font20, x + 620, y + 50, m_albumOverrideKey.overrideByDefault ? currTheme.selectedColor : Gui::makeColor(0xB8, 0xBB, 0xC2, 0xFF), m_albumOverrideKey.overrideByDefault ? "On" : "Off", ALIGNED_LEFT);
  }, [&](u32 kdown, bool *isActivated){
    if (kdown & KEY_A)
      Gui::g_nextGui = GUI_SM_SELECT;
  }, { 2, -1, 0, 4 }, false, []() -> bool {return true;});

  new Button(735, 400, 335, 80, [&](Gui *gui, u16 x, u16 y, bool *isActivated){
    gui->drawRectangled(x, y, 335, 80, currTheme.selectedColor);
    gui->drawTextAligned(font20, x + 55, y + 50, COLOR_BLACK, "Reboot to Hekate", ALIGNED_LEFT);
    gui->drawTextAligned(font20, x + 620, y + 50, m_albumOverrideKey.overrideByDefault ? currTheme.selectedColor : Gui::makeColor(0xB8, 0xBB, 0xC2, 0xFF), m_albumOverrideKey.overrideByDefault ? "On" : "Off", ALIGNED_LEFT);
  }, [&](u32 kdown, bool *isActivated){
    if (kdown & KEY_A)
      Gui::g_nextGui = GUI_HEKATE;
  }, { 2, -1, 3, -1 }, false, []() -> bool {return true;});
}

GuiMain::~GuiMain() {
  Button::g_buttons.clear();
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
  Gui::drawTextAligned(font24, 70, 58, currTheme.textColor, "        Kosmos Toolbox", ALIGNED_LEFT);
  Gui::drawTextAligned(font20, Gui::g_framebuffer_width - 50, Gui::g_framebuffer_height - 25, currTheme.textColor, "\uE0E1 Back     \uE0E0 Ok", ALIGNED_RIGHT);
  Gui::drawTextAligned(font24, Gui::g_framebuffer_width / 2, Gui::g_framebuffer_height - 130, currTheme.textColor, "Press \uE044 to save and return back to the home menu", ALIGNED_CENTER);

  for(Button *btn : Button::g_buttons)
    btn->draw(this);
  Gui::endDraw();
}

void GuiMain::onInput(u32 kdown) {
  for(Button *btn : Button::g_buttons) {
    if (btn->isSelected())
      if (btn->onInput(kdown)) return;
  }    
    
  if (kdown & KEY_B && !exitDisabled)
    g_exitApplet = true;

}

void GuiMain::onTouch(touchPosition &touch) {
  for(Button *btn : Button::g_buttons) {
    btn->onTouch(touch);
  }
}

void GuiMain::onGesture(touchPosition &startPosition, touchPosition &endPosition) {

}
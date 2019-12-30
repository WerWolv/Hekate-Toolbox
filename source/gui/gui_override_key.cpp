#include "gui_override_key.hpp"
#include "button.hpp"
#include "utils.hpp"
#include "SimpleIniParser.hpp"

#include "list_selector.hpp"
#include "override_key.hpp"

GuiOverrideKey::GuiOverrideKey() : Gui() {
  loadConfigFile();

  //0
  new Button(640, 200, 380, 200, [&](Gui *gui, u16 x, u16 y, bool *isActivated){
    gui->drawTextAligned(fontHuge, x + 190, y + 145, currTheme.textColor, OverrideKey::KeyToUnicode(m_override.key.key), ALIGNED_CENTER);
    gui->drawTextAligned(font14, x + 190, y + 185, currTheme.textColor, "Override key", ALIGNED_CENTER);
  }, [&](u64 kdown, bool *isActivated){
      if(*isActivated) {

        // This is supposed to clear the key display, and block exit until a button is pressed.
        // For some reason, it doesn't work
        this->m_override.key.key = static_cast<HidControllerKeys>(0);
        this->m_inputBlocked = true;
        if(!(kdown & (kdown - 1)) && (kdown <= KEY_DDOWN || kdown >= KEY_SL) && kdown != KEY_TOUCH) {
          m_override.key.key = static_cast<HidControllerKeys>(kdown);
          //Find or create a loader ini file with set override_key values, and write the result to the file.
          simpleIniParser::Ini *ini = simpleIniParser::Ini::parseOrCreateFile(LOADER_INI);
          auto keyValue = m_override.key.ToString();
          ini->findOrCreateSection("hbl_config", true, simpleIniParser::IniSectionType::Section)->findOrCreateFirstOption(OverrideKey::getOverrideKeyString(g_keyType), "")->value = keyValue;

          ini->writeToFile(LOADER_INI);
          *isActivated = false;

          delete ini;
          this->m_inputBlocked = false;
        }
      }
  }, { -1, 1, 2, -1 }, true, []() -> bool {return true;});

  //1
  new Button(640, 420, 380, 80, [&](Gui *gui, u16 x, u16 y, bool *isActivated){
     gui->drawTextAligned(font20, x + 20, y + 50, currTheme.textColor, "Key must be:", ALIGNED_LEFT);
     gui->drawTextAligned(font20, x + 360, y + 50, currTheme.selectedColor, m_override.key.overrideByDefault ? "Unpressed" : "Pressed", ALIGNED_RIGHT);
   }, [&](u32 kdown, bool *isActivated){
     if (kdown & KEY_A) {
        m_override.key.overrideByDefault = !m_override.key.overrideByDefault;
        if (m_override.key.key == static_cast<HidControllerKeys>(0))
          return;

        //Find or create a loader ini file with set override_key values, and write the result to the file.
        simpleIniParser::Ini *ini = simpleIniParser::Ini::parseOrCreateFile(LOADER_INI);
        auto keyValue = m_override.key.ToString();
        ini->findOrCreateSection("hbl_config", true, simpleIniParser::IniSectionType::Section)->findOrCreateFirstOption(OverrideKey::getOverrideKeyString(g_keyType), "")->value = keyValue;

        ini->writeToFile(LOADER_INI);
        delete ini;
      }
   }, { 0, (g_keyType == OverrideKeyType::AnyAppOverride ? 3 : -1), 2, -1 }, false, []() -> bool {return true;});


  switch (g_keyType)
  {
  case OverrideKeyType::Override0:
    //2
    new Button(220, 200, 300, 300, [&](Gui *gui, u16 x, u16 y, bool *isActivated){
    gui->drawTextAligned(fontHuge, x + 150, y + 190, currTheme.textColor, "\uE134", ALIGNED_CENTER);
    gui->drawTextAligned(font24, x, y - 60, currTheme.textColor, "Override when entering:", ALIGNED_LEFT);
    gui->drawTextAligned(font24, x, y - 20, currTheme.textColor, "Album", ALIGNED_LEFT);
    }, [&](u64 kdown, bool *isActivated){}, { -1, 1, -1, 0 }, false, []() -> bool {return false;});
    break;
  case OverrideKeyType::AnyAppOverride:
    //2
    new Button(220, 200, 300, 300, [&](Gui *gui, u16 x, u16 y, bool *isActivated){
    gui->drawTextAligned(fontHuge, x + 150, y + 190, currTheme.textColor, "\uE135", ALIGNED_CENTER);
    gui->drawTextAligned(font24, x, y - 60, currTheme.textColor, "Override when entering:", ALIGNED_LEFT);
    gui->drawTextAligned(font24, x, y - 20, currTheme.textColor, "Any title", ALIGNED_LEFT);
    }, [&](u64 kdown, bool *isActivated){}, { -1, -1, -1, 0 }, false, []() -> bool {return false;});

    //3
    new Button(640, 520, 380, 80, [&](Gui *gui, u16 x, u16 y, bool *isActivated){
    gui->drawTextAligned(font20, x + 20, y + 50, currTheme.textColor, "Enabled", ALIGNED_LEFT);
    gui->drawTextAligned(font20, x + 360, y + 50, m_overrideAnyApp ? currTheme.selectedColor : currTheme.unselectedColor, m_overrideAnyApp ? "On" : "Off", ALIGNED_RIGHT);
    }, [&](u64 kdown, bool *isActivated) {
     if (kdown & KEY_A) {
        m_overrideAnyApp = !m_overrideAnyApp;

        //Find or create a loader ini file with set override_key values, and write the result to the file.
        simpleIniParser::Ini *ini = simpleIniParser::Ini::parseOrCreateFile(LOADER_INI);
        ini->findOrCreateSection("hbl_config", true, simpleIniParser::IniSectionType::Section)->findOrCreateFirstOption("override_any_app", "")->value = m_overrideAnyApp ? "true" : "false";

        ini->writeToFile(LOADER_INI);
        delete ini;
      }
   }, { 1, -1, 2, -1 }, false, []() -> bool {return true;});
    break;
  default:
    //2
    new Button(220, 200, 300, 300, [&](Gui *gui, u16 x, u16 y, bool *isActivated){
    gui->drawTextAligned(fontHuge, x + 150, y + 190, currTheme.textColor, "\uE06B", ALIGNED_CENTER);
    gui->drawTextAligned(font24, x, y - 60, currTheme.textColor, "Override when entering:", ALIGNED_LEFT);
    gui->drawTextAligned(font24, x, y - 20, currTheme.textColor, "Custom title", ALIGNED_LEFT);
    }, [&](u64 kdown, bool *isActivated){}, { -1, 1, -1, 0 }, false, []() -> bool {return false;});
    break;
  }
}

GuiOverrideKey::~GuiOverrideKey() {
  Button::g_buttons.clear();
}

void GuiOverrideKey::update() {
  Gui::update();
}

void GuiOverrideKey::draw() {
  Gui::beginDraw();
  Gui::drawRectangle(0, 0, Gui::g_framebuffer_width, Gui::g_framebuffer_height, currTheme.backgroundColor);
  Gui::drawRectangle((u32)((Gui::g_framebuffer_width - 1220) / 2), 87, 1220, 1, currTheme.textColor);
  Gui::drawRectangle((u32)((Gui::g_framebuffer_width - 1220) / 2), Gui::g_framebuffer_height - 73, 1220, 1, currTheme.textColor);
  Gui::drawTextAligned(fontIcons, 70, 68, currTheme.textColor, "\uE130", ALIGNED_LEFT);
  Gui::drawTextAligned(font24, 70, 58, currTheme.textColor, "        Application override settings", ALIGNED_LEFT);
  Gui::drawTextAligned(font20, Gui::g_framebuffer_width - 50, Gui::g_framebuffer_height - 25, currTheme.textColor, "\uE0E1 Back     \uE0E0 OK", ALIGNED_RIGHT);

  for(Button *btn : Button::g_buttons)
    btn->draw(this);
  Gui::endDraw();
}

void GuiOverrideKey::onInput(u32 kdown) {
  for(Button *btn : Button::g_buttons) {
    if (btn->isSelected())
      if (btn->onInput(kdown)) return;
  }    
    
  if (kdown & KEY_B && !m_inputBlocked)
    Gui::g_nextGui = GUI_OVERRIDES_MENU;
}

void GuiOverrideKey::onTouch(touchPosition &touch) {
  for(Button *btn : Button::g_buttons) {
    btn->onTouch(touch);
  }
}

void GuiOverrideKey::onGesture(touchPosition &startPosition, touchPosition &endPosition) {

}

void GuiOverrideKey::loadConfigFile()  {
  // Get the override keys, if any exist
  simpleIniParser::Ini *ini = simpleIniParser::Ini::parseOrCreateFile(LOADER_INI);
  m_override.key = OverrideKey::StringToKeyCombo(ini->findOrCreateSection("hbl_config", true, simpleIniParser::IniSectionType::Section)
  ->findOrCreateFirstOption(OverrideKey::getOverrideKeyString(g_keyType), g_keyType == OverrideKeyType::AnyAppOverride ? "R" : "!R")->value);
  auto option = ini->findOrCreateSection("hbl_config", true, simpleIniParser::IniSectionType::Section)->findFirstOption("override_any_app");
  if (option != nullptr)
    m_overrideAnyApp = (option->value == "true") || (option->value == "1");
  else
    m_overrideAnyApp = true;
  delete ini;
}
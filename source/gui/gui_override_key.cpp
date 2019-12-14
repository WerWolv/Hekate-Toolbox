#include "gui_override_key.hpp"
#include "button.hpp"
#include "utils.hpp"
#include "SimpleIniParser.hpp"

#include "list_selector.hpp"
#include "override_key.hpp"

GuiOverrideKey::GuiOverrideKey() : Gui() {
  {
    // Get the override keys, if any exist
    simpleIniParser::Ini *ini = simpleIniParser::Ini::parseOrCreateFile(LOADER_INI);
    m_override.key = OverrideKey::StringToKeyCombo(ini->findOrCreateSection("hbl_config", true, simpleIniParser::IniSectionType::Section)->findOrCreateFirstOption(getOverrideKeyString(g_keyType), "")->value);
    delete ini;
  }

  //0
  new Button(150, 200, 200, 200, [&](Gui *gui, u16 x, u16 y, bool *isActivated){
    gui->drawTextAligned(fontHuge, x + 37, y + 145, currTheme.textColor, OverrideKey::KeyToUnicode(m_override.key.key), ALIGNED_LEFT);
    gui->drawTextAligned(font14, x + 100, y + 185, currTheme.textColor, "Override key", ALIGNED_CENTER);
  }, [&](u64 kdown, bool *isActivated){
    m_inputBlocked = true;
    if(*isActivated) {
      if(!(kdown & (kdown - 1)) && (kdown <= KEY_DDOWN || kdown >= KEY_SL) && kdown != KEY_TOUCH) {
        m_override.key.key = static_cast<HidControllerKeys>(kdown);
        //Find or create a loader ini file with set override_key values, and write the result to the file.
        simpleIniParser::Ini *ini = simpleIniParser::Ini::parseOrCreateFile(LOADER_INI);
        auto keyValue = m_override.key.ToString();
        ini->findOrCreateSection("hbl_config", true, simpleIniParser::IniSectionType::Section)->findOrCreateFirstOption(getOverrideKeyString(g_keyType), "")->value = keyValue;

        ini->writeToFile(LOADER_INI);
        *isActivated = false;

        delete ini;
      }
    }
    m_inputBlocked = false;
  }, { -1, -1, -1, 1 }, true, []() -> bool {return true;});

  //1
  new Button(370, 200, 700, 80, [&](Gui *gui, u16 x, u16 y, bool *isActivated){
     gui->drawTextAligned(font20, x + 37, y + 50, currTheme.textColor, "Key must be", ALIGNED_LEFT);
     gui->drawTextAligned(font20, x + 663, y + 50, currTheme.selectedColor, m_override.key.overrideByDefault ? "Unpressed" : "Pressed", ALIGNED_RIGHT);
   }, [&](u32 kdown, bool *isActivated){
     if (kdown & KEY_A) {
        m_override.key.overrideByDefault = !m_override.key.overrideByDefault;
        //Find or create a loader ini file with set override_key values, and write the result to the file.
        simpleIniParser::Ini *ini = simpleIniParser::Ini::parseOrCreateFile(LOADER_INI);
        auto keyValue = m_override.key.ToString();
        ini->findOrCreateSection("hbl_config", true, simpleIniParser::IniSectionType::Section)->findOrCreateFirstOption(getOverrideKeyString(g_keyType), "")->value = keyValue;

        ini->writeToFile(LOADER_INI);
        delete ini;
      }
   }, { -1, 2, 0, -1 }, false, []() -> bool {return true;});
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
  Gui::drawTextAligned(font20, Gui::g_framebuffer_width - 50, Gui::g_framebuffer_height - 25, currTheme.textColor, "\uE0E1 Back     \uE0E0 Ok", ALIGNED_RIGHT);

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


const char* GuiOverrideKey::getOverrideKeyString(OverrideKeyType type) {
  switch (type) {
  case OverrideKeyType::AnyAppOverride:
    return "override_any_app_key";
  case OverrideKeyType::Override0:
    return "override_key";
  case OverrideKeyType::Override1:
    return "override_key_1";
  case OverrideKeyType::Override2:
    return "override_key_2";
  case OverrideKeyType::Override3:
    return "override_key_3";
  case OverrideKeyType::Override4:
    return "override_key_4";
  case OverrideKeyType::Override5:
    return "override_key_5";
  case OverrideKeyType::Override6:
    return "override_key_6";
  case OverrideKeyType::Override7:
    return "override_key_7";
  default:
    return "";
  }
}

const char* GuiOverrideKey::getOverrideProgramString(OverrideKeyType type) {
  switch (type) {
  case OverrideKeyType::Override0:
    return "program_id";
  case OverrideKeyType::Override1:
    return "program_id_1";
  case OverrideKeyType::Override2:
    return "program_id_2";
  case OverrideKeyType::Override3:
    return "program_id_3";
  case OverrideKeyType::Override4:
    return "program_id_4";
  case OverrideKeyType::Override5:
    return "program_id_5";
  case OverrideKeyType::Override6:
    return "program_id_6";
  case OverrideKeyType::Override7:
    return "program_id_7";
  default:
    return "";
  }
}

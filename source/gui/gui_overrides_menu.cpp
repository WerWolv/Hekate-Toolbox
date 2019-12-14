#include "gui_overrides_menu.hpp"
#include "gui_override_key.hpp"
#include "button.hpp"
#include "utils.hpp"
#include "SimpleIniParser.hpp"

#include "utils.hpp"
#include "list_selector.hpp"
#include "override_key.hpp"

GuiOverridesMenu::GuiOverridesMenu() : Gui() {
  Button::g_buttons.clear();
  loadConfigFile();

  for (int i=0; i!=8; ++i) {
    if (m_overrides[i].programID != ProgramID::Invalid) {
      if (m_overrides[i].programID == ProgramID::AppletPhotoViewer)
        addButton(OverrideButtonType::Album, static_cast<OverrideKeyType>(i), m_overrides[i]);
      else
        addButton(OverrideButtonType::Custom_Title, static_cast<OverrideKeyType>(i), m_overrides[i]);
    }
  }

  if (m_overrideAnyApp)
    if (m_anyAppOverride.key.key != static_cast<HidControllerKeys>(0))
      addButton(OverrideButtonType::Any_Title, OverrideKeyType::AnyAppOverride, m_anyAppOverride);
}

GuiOverridesMenu::~GuiOverridesMenu() {
  Button::g_buttons.clear();
}

void GuiOverridesMenu::update() {
  Gui::update();
}

void GuiOverridesMenu::draw() {
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

void GuiOverridesMenu::onInput(u32 kdown) {
  for(Button *btn : Button::g_buttons) {
    if (btn->isSelected())
      if (btn->onInput(kdown)) return;
  }    
    
  if (kdown & KEY_B)
    Gui::g_nextGui = GUI_MAIN;
}

void GuiOverridesMenu::onTouch(touchPosition &touch) {
  for(Button *btn : Button::g_buttons) {
    btn->onTouch(touch);
  }
}

void GuiOverridesMenu::onGesture(touchPosition &startPosition, touchPosition &endPosition) {

}

void GuiOverridesMenu::addButton(OverrideButtonType buttonType, OverrideKeyType keyType, const ProgramOverrideKey &key) {
  std::function<void(Gui*, u16, u16, bool*)> drawAction;
  switch (buttonType)
  {
  case OverrideButtonType::Album:
    drawAction = [&](Gui *gui, u16 x, u16 y, bool *isActivated){
      gui->drawTextAligned(fontHuge, x + 100, y + 150, currTheme.textColor, "\uE134", ALIGNED_CENTER);
      gui->drawTextAligned(font24, x + 100, y + 285, currTheme.textColor, "Album", ALIGNED_CENTER);
    };
    break;
  case OverrideButtonType::Any_Title:
    drawAction = [&](Gui *gui, u16 x, u16 y, bool *isActivated){
      gui->drawTextAligned(fontHuge, x + 100, y + 150, currTheme.textColor, "\uE135", ALIGNED_CENTER);
      gui->drawTextAligned(font24, x + 100, y + 285, currTheme.textColor, "Any title", ALIGNED_CENTER);
    };
    break;
  case OverrideButtonType::Custom_Title:
    drawAction = [&](Gui *gui, u16 x, u16 y, bool *isActivated){
      gui->drawTextAligned(fontHuge, x + 100, y + 150, currTheme.textColor, "\uE131", ALIGNED_CENTER);
      gui->drawTextAligned(font24, x + 100, y + 285, currTheme.textColor, "Custom title", ALIGNED_CENTER);
    };
    break;
  default:
    break;
  }
  new Button((220*m_buttonCount)+150, 200, 200, 300, drawAction, [&, keyType, key](u64 kdown, bool *isActivated){
    if (kdown & KEY_A) {
      GuiOverrideKey::g_overrideKey = key;
      GuiOverrideKey::g_keyType = keyType;
      Gui::g_nextGui = GUI_OVERRIDE_KEY;
    }
  }, { -1, -1, m_buttonCount-1, m_buttonCount+1 }, false, []() -> bool {return true;});

  m_buttonCount++;
}

void GuiOverridesMenu::loadConfigFile() {
  // If it doesn't find the config with a section [hbl_config], it stops, as there is nothing more to read.
  simpleIniParser::Ini *ini = simpleIniParser::Ini::parseOrCreateFile(LOADER_INI);
  auto section = ini->findSection("hbl_config", true, simpleIniParser::IniSectionType::Section);

  if (section == nullptr)
    return;

  // Get the override key and program for un-numbered override
  auto option = section->findFirstOption("override_key");
  if (option != nullptr) {
    m_overrides[0].key = OverrideKey::StringToKeyCombo(option->value);
    option = section->findFirstOption("program_id");
    if (option != nullptr)
      m_overrides[0].programID = strtoul(option->value.c_str(), nullptr, 16);
    else
      m_overrides[0].programID = ProgramID::AppletPhotoViewer;
  }

  // Get the override key if config is set to override any app
  option = section->findFirstOption("override_any_app");
  if (option != nullptr)
    m_overrideAnyApp = (option->value == "true" || option->value == "1");

  if (m_overrideAnyApp) {
    option = section->findFirstOption("override_any_app_key");
    if (option != nullptr) {
      m_anyAppOverride.key = OverrideKey::StringToKeyCombo(option->value);
    }
  }

  // Get the override keys and programs for numbered overrides
  for (u8 i=0; i!=8; ++i) {

    option = section->findFirstOption("override_key_" + i);
    if (option != nullptr)
      m_overrides[i].key = OverrideKey::StringToKeyCombo(option->value);

    option = section->findFirstOption("program_id_" + i);
    if (option != nullptr)
      m_overrides[i].programID = strtoul(option->value.c_str(), nullptr, 16);

  }
  delete ini;
}

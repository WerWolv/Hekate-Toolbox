#include "gui_overrides_menu.hpp"
#include "gui_override_key.hpp"
#include "button.hpp"
#include "utils.hpp"
#include "SimpleIniParser.hpp"

#include "utils.hpp"
#include "list_selector.hpp"
#include "message_box.hpp"
#include "override_key.hpp"
#include "titleinfo.hpp"
#include <algorithm>

GuiOverridesMenu::GuiOverridesMenu() : Gui() {
  Button::g_buttons.clear();
  loadConfigFile();

  displayDefaultOption = true;
  displayAnyTitleOption = true;

  if (m_anyAppOverride.key.key != static_cast<HidControllerKeys>(0))
    addButton(OverrideButtonType::Any_Title, OverrideKeyType::AnyAppOverride, m_anyAppOverride);

  for (int i=0; i!=8; ++i) {
    if (m_overrides[i].programID != AppletID::Invalid 
    || (m_overrides[i].key.key != static_cast<HidControllerKeys>(0))) {
      addButton(OverrideButtonType::Custom_Title, static_cast<OverrideKeyType>(i), m_overrides[i]);
    }
  }


  if (m_addConfigs.size() != 0 || displayDefaultOption || displayAnyTitleOption)
    addButton(OverrideButtonType::Add_New);

  Button::select(selection);
}

GuiOverridesMenu::~GuiOverridesMenu() {
  selection = Button::getSelectedIndex();
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
  Gui::drawTextAligned(font20, Gui::g_framebuffer_width - 50, Gui::g_framebuffer_height - 25, currTheme.textColor, "\uE0E2 Delete     \uE0E1 Back     \uE0E0 OK", ALIGNED_RIGHT);
  Gui::drawTextAligned(font20, Gui::g_framebuffer_width / 2, 150, currTheme.textColor, "Add key override options for each title. \n Opening that title while holding that key combination will override it with hbmenu.", ALIGNED_CENTER);

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

  if (kdown & KEY_X) {
    //Get the button options based on selection
    auto tuple = m_buttons[Button::getSelectedIndex()];
    auto buttonType = std::get<0>(tuple);
    auto keyType = std::get<1>(tuple);
    if (buttonType != OverrideButtonType::Add_New) {
      (new MessageBox("Are you sure you want to delete this setting?", MessageBox::YES_NO))->setSelectionAction([&, keyType](s8 selectedItem){
        if (selectedItem == MessageBox::BUTTON_YES) {
          //Parse INI file and remove any occurences of wanted options
          auto ini = simpleIniParser::Ini::parseFile(LOADER_INI);
          if (ini != nullptr) {
            auto section = ini->findSection(HBL_CONFIG, true, simpleIniParser::IniSectionType::Section);
            if (section != nullptr) {
              auto option = section->findFirstOption(OverrideKey::getOverrideKeyString(keyType));
              if (option != nullptr) {
                auto &options = section->options;
                options.erase(std::remove(options.begin(), options.end(), option), options.end());
              }
              option = section->findFirstOption(OverrideKey::getOverrideProgramString(keyType));
              if (option != nullptr) {
                auto &options = section->options;
                options.erase(std::remove(options.begin(), options.end(), option), options.end());
              }
              if (keyType == OverrideKeyType::Default) {
                option = section->findFirstOption(OVERRIDE_KEY);
                if (option != nullptr) {
                  auto &options = section->options;
                  options.erase(std::remove(options.begin(), options.end(), option), options.end());
                }
                option = section->findFirstOption(PROGRAM_ID);
                if (option != nullptr) {
                  auto &options = section->options;
                  options.erase(std::remove(options.begin(), options.end(), option), options.end());
                }
              }
              if (keyType == OverrideKeyType::AnyAppOverride) {
                option = section->findFirstOption("override_any_app");
                if (option != nullptr) {
                  auto &options = section->options;
                  options.erase(std::remove(options.begin(), options.end(), option), options.end());
                }
              }

              //Write the resulting ini back in its place
              ini->writeToFile(LOADER_INI);
              delete ini;
            }
          }
          //Reload the GUI menu because i can't be bothered to remove buttons manually
          Gui::g_nextGui = GUI_OVERRIDES_MENU;
        }
        Gui::g_currMessageBox->hide();
      })->show();
    }
  }
}

void GuiOverridesMenu::onTouch(touchPosition &touch) {
  for(Button *btn : Button::g_buttons) {
    btn->onTouch(touch);
  }
}

void GuiOverridesMenu::onGesture(touchPosition &startPosition, touchPosition &endPosition) {

}

void GuiOverridesMenu::addButton(OverrideButtonType buttonType, OverrideKeyType keyType, const ProgramOverrideKey &key) {
  static std::vector<std::string> configNames;
  configNames.reserve(OVERRIDEKEY_TYPES);
  std::function<void(Gui*, u16, u16, bool*)> drawAction;
  std::function<void(u32, bool*)> inputAction = [&, keyType, key](u64 kdown, bool *isActivated){
    if (kdown & KEY_A) {
      GuiOverrideKey::g_overrideKey = key;
      GuiOverrideKey::g_keyType = keyType;
      Gui::g_nextGui = GUI_OVERRIDE_KEY;
    }
  };
  switch (buttonType)
  {
  case OverrideButtonType::Any_Title:
    drawAction = [&](Gui *gui, u16 x, u16 y, bool *isActivated){
      gui->drawTextAligned(fontHuge, x + 100, y + 150, currTheme.textColor, "\uE135", ALIGNED_CENTER);
      gui->drawTextAligned(font24, x + 100, y + 285, currTheme.textColor, "Any title", ALIGNED_CENTER);
    };
    break;
  case OverrideButtonType::Custom_Title:
    drawAction = [&, keyType, title{DumpTitle(key.programID, WidthHeight{192, 192})}](Gui *gui, u16 x, u16 y, bool *isActivated){

      if(title != nullptr && title->application_id == 0 && keyType == OverrideKeyType::Default)
        title->application_id = AppletID::AppletPhotoViewer;

      if(title != nullptr && title->application_id != 0) {

        if(title->icon.get() != nullptr) {
          gui->drawShadow(x+4, y+4, 192, 192);
          gui->drawImage(x+4, y+4, 192, 192, title->icon.get(), ImageMode::IMAGE_MODE_RGBA32);
        } else {
          gui->drawTextAligned(fontHuge, x + 100, y + 150, GetAppletColor(title->application_id), GetAppletIcon(title->application_id), ALIGNED_CENTER);
        }

      } else {
        gui->drawTextAligned(fontHuge, x + 100, y + 150, currTheme.textColor, "\uE06B", ALIGNED_CENTER);
      }
      gui->drawTextAligned(font24, x + 100, y + 285, currTheme.textColor, keyType == OverrideKeyType::Default ? "Default" : "Custom title", ALIGNED_CENTER);
    };
    break;
  case OverrideButtonType::Add_New:
    drawAction = [&](Gui *gui, u16 x, u16 y, bool *isActivated){
      gui->drawTextAligned(fontHuge, x + 100, y + 150, currTheme.unselectedColor, "\uE0F1", ALIGNED_CENTER);
      gui->drawTextAligned(font24, x + 100, y + 285, currTheme.unselectedColor, "Add...", ALIGNED_CENTER);
    };

    inputAction = [&](u64 kdown, bool *isActivated){
      if (kdown & KEY_A) {
        configNames.clear();

        if (displayAnyTitleOption)
          configNames.push_back("Any Title");

        if (displayDefaultOption)
          configNames.push_back("Change default");

        if (m_addConfigs.size() != 0)
          configNames.push_back("Custom title");

        (new ListSelector("Add new key override for:", "\uE0E1 Back     \uE0E0 OK", configNames, 0))->setInputAction([&](u32 k, u16 selectedItem){
          if(k & KEY_A) {

            auto newKeyType = m_addConfigs[0];

            if (displayAnyTitleOption && selectedItem == 0)
              newKeyType = OverrideKeyType::AnyAppOverride;
            if (displayDefaultOption && selectedItem == displayAnyTitleOption)
              newKeyType = OverrideKeyType::Default;

            GuiOverrideKey::g_keyType = newKeyType;
            Gui::g_currListSelector->hide();
            Gui::g_nextGui = GUI_OVERRIDE_KEY;
          }
        })->show();
      }
    };
  default:
    break;
  }
  new Button((220*m_buttonCount)+150, 250, 200, 300, drawAction, inputAction,
    { -1, -1, m_buttonCount-1, m_buttonCount+1 }, false, []() -> bool {return true;});

  if (keyType == OverrideKeyType::Default)
    displayDefaultOption = false;
  else if (keyType == OverrideKeyType::AnyAppOverride)
    displayAnyTitleOption = false;
  else
    removeFromList(keyType);

  m_buttons.push_back(std::tuple(buttonType,keyType));
  m_buttonCount++;
}

void GuiOverridesMenu::loadConfigFile() {
  for (int i=1; i!= 8; ++i) {
    m_addConfigs.push_back(static_cast<OverrideKeyType>(i));
  }
  // If it doesn't find the config with a section [hbl_config], it stops, as there is nothing more to read.
  simpleIniParser::Ini *ini = simpleIniParser::Ini::parseOrCreateFile(LOADER_INI);
  auto section = ini->findSection(HBL_CONFIG, true, simpleIniParser::IniSectionType::Section);

  if (section == nullptr)
    return;

  // Get the override key and program for un-numbered override
  // TODO: this may be removed in a future atmosphere release
  auto option = section->findFirstOption(OVERRIDE_KEY);
  if (option != nullptr)
    m_overrides[0].key = OverrideKey::StringToKeyCombo(option->value);

  option = section->findFirstOption(PROGRAM_ID);
  if (option != nullptr)
    m_overrides[0].programID = strtoul(option->value.c_str(), nullptr, 16);

  // Get the override key if config is set to override any app
  option = section->findFirstOption("override_any_app");
  if (option != nullptr)
    m_overrideAnyApp = (option->value == "true" || option->value == "1");
  else
    m_overrideAnyApp = true;

  option = section->findFirstOption("override_any_app_key");
  if (option != nullptr) {
    m_anyAppOverride.key = OverrideKey::StringToKeyCombo(option->value);
  }

  // Get the override keys and programs for numbered overrides
  for (u8 i=0; i!=8; ++i) {

    option = section->findFirstOption(OverrideKey::getOverrideKeyString(static_cast<OverrideKeyType>(i)));
    if (option != nullptr)
      m_overrides[i].key = OverrideKey::StringToKeyCombo(option->value);

    option = section->findFirstOption(OverrideKey::getOverrideProgramString(static_cast<OverrideKeyType>(i)));
    if (option != nullptr)
      m_overrides[i].programID = strtoul(option->value.c_str(), nullptr, 16);

  }
  delete ini;
}

void GuiOverridesMenu::removeFromList(OverrideKeyType keyEntry) {
  m_addConfigs.erase(std::remove(m_addConfigs.begin(), m_addConfigs.end(), keyEntry), m_addConfigs.end());
}
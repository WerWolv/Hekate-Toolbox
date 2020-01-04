#include "gui_title_list.hpp"
#include "button.hpp"
#include "utils.hpp"
#include <memory>
#include <string>
#include <cstring>
#include "gui_override_key.hpp"
#include "gametitle.hpp"
#include "SimpleIniParser.hpp"
#include <cstdlib>

GuiTitleList::GuiTitleList() : Gui() {
  auto apps = DumpAllGames();
  size_t position = 0;
  int buttonIndex = 0;

  for (auto &&app : apps) {

    new Button(107 + position, 194, 256, 256,

    [app](Gui *gui, u16 x, u16 y, bool *isActivated){
      if (app->icon.get() != nullptr)
        gui->drawImage(x, y, 256, 256, app->icon.get(), ImageMode::IMAGE_MODE_RGBA32);
      if (*isActivated) {
        gui->drawTextAligned(font14, x + 128, y - 32, currTheme.textColor, app->name, ALIGNED_CENTER);
      }
    },

    [&, app](u64 kdown, bool *isActivated){
      if (kdown & KEY_A) {

        //convert title id to a hex string
        char buffer[17];
        sprintf(buffer, "%016lx", app->application_id);

        //write title id to config
        simpleIniParser::Ini *ini = simpleIniParser::Ini::parseOrCreateFile(LOADER_INI);
        ini->findOrCreateSection(HBL_CONFIG, true, simpleIniParser::IniSectionType::Section)
          ->findOrCreateFirstOption(OverrideKey::getOverrideProgramString(GuiOverrideKey::g_keyType), "")
          ->value = buffer;

        ini->writeToFile(LOADER_INI);
        delete ini;
        Gui::g_nextGui = GUI_OVERRIDE_KEY;
      }
    }, { -1, -1, buttonIndex - 1, buttonIndex + 1 }, false, []() -> bool {return true;});
    
    position += 256 + 14;
    buttonIndex++;
  }
}

GuiTitleList::~GuiTitleList() {
  Button::g_buttons.clear();
}

void GuiTitleList::update() {
  Gui::update();
}

void GuiTitleList::draw() {
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

void GuiTitleList::onInput(u32 kdown) {
  for(Button *btn : Button::g_buttons) {
    if (btn->isSelected())
      if (btn->onInput(kdown)) return;
  }

  if (kdown & KEY_B)
    Gui::g_nextGui = GUI_OVERRIDE_KEY;
}

void GuiTitleList::onTouch(touchPosition &touch) {
  for(Button *btn : Button::g_buttons) {
    btn->onTouch(touch);
  }
}

void GuiTitleList::onGesture(touchPosition &startPosition, touchPosition &endPosition) {

}
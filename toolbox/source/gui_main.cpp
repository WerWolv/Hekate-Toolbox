#include "gui_main.hpp"
#include "button.hpp"

extern "C" {
  #include "ldr_cfg.h"
}

GuiMain::GuiMain() : Gui() {
  m_loaderIni = new mINI::INIFile("sdmc:/atmosphere/loader.ini");
  m_loaderIni->read(m_ini);

  ldrCfgInitialize();

  getCurrOverrideKeyCombo(&m_overrideKeyCombo);
  getOverrideByDefault(&m_overrideByDefault);

  m_editingKey = false;
  new Button(150, 240, 200, 200, [&](Gui *gui, u16 x, u16 y, bool *isActivated){
    gui->drawTextAligned(fontHuge, x + 37, y + 145, currTheme.textColor, keyToUnicode(m_overrideKeyCombo), ALIGNED_LEFT);
  }, [&](u32 kdown, bool *isActivated){
    if(*isActivated) {
      if(!(kdown & (kdown - 1)) && (kdown <= KEY_DDOWN || kdown >= KEY_SL)) {
        setCurrOverrideKeyCombo(kdown, &m_overrideKeyCombo);
        m_ini["config"]["override_key"] = GuiMain::keyToKeyChars(m_overrideKeyCombo, m_overrideByDefault);
        m_loaderIni->write(m_ini, true);
        *isActivated = false;
      }
    }
   });
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
    case KEY_TOUCH:   return "\uE058";
    default:          return "\uE152";
  }
}

std::string GuiMain::keyToKeyChars(u64 key, bool overrideByDefault) {
  std::string ret = !overrideByDefault ? "!" : "";
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

void GuiMain::update() {
  Gui::update();
}

void GuiMain::draw() {
  Gui::beginDraw();

  Gui::drawRectangle(0, 0, Gui::g_framebuffer_width, Gui::g_framebuffer_height, currTheme.backgroundColor);
  Gui::drawRectangle((u32)((Gui::g_framebuffer_width - 1220) / 2), Gui::g_framebuffer_height - 73, 1220, 1, currTheme.textColor);
  Gui::drawTextAligned(font20, Gui::g_framebuffer_width - 50, Gui::g_framebuffer_height - 25, currTheme.textColor, "\uE0E1 Back     \uE0E0 Ok", ALIGNED_RIGHT);

  for(Button *btn : Button::g_buttons)
    btn->draw(this);

  Gui::endDraw();
}

void GuiMain::onInput(u32 kdown) {
  if (m_editingKey) {
    if(!(kdown & (kdown - 1)) && (kdown <= KEY_DDOWN || kdown >= KEY_SL) && kdown != KEY_TOUCH) {
      setCurrOverrideKeyCombo(kdown, &m_overrideKeyCombo);
      m_editingKey = false;
    }
  } else {
    if(kdown & KEY_A) {
      m_editingKey = true;
    }
  }

  for(Button *btn : Button::g_buttons)
    btn->onInput(kdown);
}

void GuiMain::onTouch(touchPosition &touch) {

}

void GuiMain::onGesture(touchPosition &startPosition, touchPosition &endPosition) {

}

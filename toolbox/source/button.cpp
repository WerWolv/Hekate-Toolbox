#include "button.hpp"

extern "C" {
  #include "theme.h"
}

Button::Button(u16 x, u16 y, u16 w, u16 h, std::function<void(Gui*, u16, u16, bool*)> drawAction, std::function<void(u32, bool*)> inputAction) : m_x(x), m_y(y), m_w(w), m_h(h), m_drawAction(drawAction), m_inputAction(inputAction) {
  Button::g_buttons.push_back(this);

  m_isActivated = false;
  m_isSelected = false;

  select(0);
}

void Button::draw(Gui *gui) {
  if(m_isSelected)
    gui->drawRectangle(m_x - 5, m_y - 5, m_w + 10, m_h + 10, m_isActivated ? currTheme.selectedColor : currTheme.highlightColor);

  gui->drawRectangle(m_x, m_y, m_w, m_h, currTheme.selectedButtonColor);
  gui->drawShadow(m_x - 5, m_y - 5, m_w + 10, m_h + 10);

  m_drawAction(gui, m_x, m_y, &m_isActivated);
}

void Button::onInput(u32 kdown) {
  if (!m_isActivated) {
    if (kdown & KEY_A) {
      m_isActivated = true;
      return;
    }
  };

  m_inputAction(kdown, &m_isActivated);

  if (kdown & KEY_B)
    m_isActivated = false;
}

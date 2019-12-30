#pragma once

#include <switch.h>

#include <functional>
#include <vector>

#include "gui.hpp"

class Button {
public:
  static inline std::vector<Button*> g_buttons;

  Button(u16 x, u16 y, u16 w, u16 h, std::function<void(Gui*, u16, u16, bool*)> drawAction, std::function<void(u32, bool*)> inputAction, std::vector<s32> adjacentButton, bool activatable, std::function<bool()> usableCondition);

  void draw(Gui *gui);
  bool onInput(u32 kdown);
  void onTouch(touchPosition &touch);

  inline bool isActivated() {
    return m_isActivated;
  }

  inline bool isSelected() {
    return m_isSelected;
  }

  static inline s16 getSelectedIndex() {
    for(size_t i=0; i != g_buttons.size(); ++i) {
      if (g_buttons[i]->m_isSelected)
        return i;
    }
    return -1;
  }

  static inline void select(s16 buttonIndex) {
    if (buttonIndex < 0) return;
    if (Button::g_buttons.size() <= static_cast<u16>(buttonIndex)) return;

    for(Button *btn : Button::g_buttons) {
      btn->m_isSelected = false;
      btn->m_isActivated = false;
    }


    Button::g_buttons[buttonIndex]->m_isSelected = true;
  }

private:

  u16 m_x, m_y, m_w, m_h;

  std::function<void(Gui*, u16, u16, bool*)> m_drawAction;
  std::function<void(u32, bool*)> m_inputAction;
  std::function<bool()> m_usableCondition;

  bool m_isActivated;
  bool m_isSelected;
  bool m_activatable;

  std::vector<s32> m_adjacentButton;
};

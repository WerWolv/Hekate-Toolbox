#pragma once

#include <switch.h>

#include <functional>
#include <vector>

#include "gui.hpp"

class Button {
public:
  static inline std::vector<Button*> g_buttons;
  static inline u16 pageOffsetX = 0;
  static inline u16 pageOffsetY = 0;
  static inline u16 targetOffsetX = 0;
  static inline u16 targetOffsetY = 0;
  static inline u16 pageLeftmostBoundary = 100;
  static inline u16 pageTopmostBoundary = 64;
  static inline u16 pageRightmostBoundary = 1280 - 100;
  static inline u16 pageBottommostBoundary = 720 - 64;

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

  static s16 getSelectedIndex();

  static void select(s16 buttonIndex);

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

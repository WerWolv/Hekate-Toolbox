#pragma once

#include "gui.hpp"


class GuiTitleList : public Gui {
public:
  GuiTitleList();
  ~GuiTitleList();

  void update();
  void draw();
  void onInput(u32 kdown);
  void onTouch(touchPosition &touch);
  void onGesture(touchPosition &startPosition, touchPosition &endPosition);
  void loadConfigFile();

  static inline u64 selectedAppID = 0;
};

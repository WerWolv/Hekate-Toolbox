#pragma once

#include "gui.hpp"

#include <vector>
#include <string>
#include <unordered_map>
#include <set>
#include "utils.hpp"
#include "override_key.hpp"


class GuiOverridesMenu : public Gui {
public:
  GuiOverridesMenu();
  ~GuiOverridesMenu();

  void update();
  void draw();
  void onInput(u32 kdown);
  void onTouch(touchPosition &touch);
  void onGesture(touchPosition &startPosition, touchPosition &endPosition);

private:
  OverrideKey m_overrideKeys[8];
  bool m_inputBlocked = false;
};

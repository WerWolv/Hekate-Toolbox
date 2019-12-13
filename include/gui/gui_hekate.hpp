#pragma once

#include "gui.hpp"

#include <vector>
#include <string>
#include <unordered_map>
#include <set>
#include "utils.hpp"


class GuiHekate : public Gui {
public:
  GuiHekate();
  ~GuiHekate();

  void update();
  void draw();
  void onInput(u32 kdown);
  void onTouch(touchPosition &touch);
  void onGesture(touchPosition &startPosition, touchPosition &endPosition);

private:
    BootEntry m_currRebootConfig;
    std::vector<BootEntry> m_rebootConfigs;
};

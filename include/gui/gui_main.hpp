#pragma once

#include "gui.hpp"

#include <vector>
#include <string>
#include <unordered_map>
#include "utils.hpp"
#include "override_key.hpp"

class GuiMain : public Gui {
public:
  GuiMain();
  ~GuiMain();

  void update();
  void draw();
  void onInput(u32 kdown);
  void onTouch(touchPosition &touch);
  void onGesture(touchPosition &startPosition, touchPosition &endPosition);

private:
  static inline int selection = 0;
  OverrideKey m_albumOverrideKey;

  BootEntry m_currAutoBootConfig;
  std::vector<BootEntry> m_autoBootConfigs;
  std::vector<std::string> m_titleNames;
  std::vector<u64> m_titleIDs;
};

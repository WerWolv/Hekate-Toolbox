#pragma once

#include "gui.hpp"

#include <vector>
#include <string>
#include <unordered_map>
#include "utils.hpp"

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
  u64 m_overrideKeyCombo;
  bool m_overrideByDefault;
  u64 m_overrideHblTid;
  bool m_sysftpdRunning;

  BootEntry m_currAutoBootConfig;
  std::vector<BootEntry> m_autoBootConfigs;
  std::vector<std::string> m_titleNames;
  std::vector<u64> m_titleIDs;

  const char* keyToUnicode(u64 key);
  std::string keyToKeyChars(u64 key, bool overrideByDefault);
  void keyCharsToKey(std::string str, u64 *key, bool *overrideByDefault);
};

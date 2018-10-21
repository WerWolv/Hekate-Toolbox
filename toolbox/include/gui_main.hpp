#pragma once

#include "gui.hpp"

#include <vector>
#include <string>
#include <unordered_map>
#include "ini/ini.hpp"

#define LOADER_INI "sdmc:/atmosphere/loader.ini"

typedef struct {
  std::string name;
  u16 id;
  bool autoBootList;
} AutoBootEntry;

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
  AutoBootEntry m_currAutoBootConfig;
  std::vector<AutoBootEntry> m_autoBootConfigs;

  const char* keyToUnicode(u64 key);
  std::string keyToKeyChars(u64 key, bool overrideByDefault);
  AutoBootEntry getAutoBootConfigs(std::vector<AutoBootEntry> &out_bootEntries, u16 &currAutoBootEntryIndex);
};

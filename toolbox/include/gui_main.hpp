#pragma once

#include "gui.hpp"
#include "ini.h"

#include <vector>
#include <unordered_map>

#define LOADER_INI "sdmc:/atmosphere/loader.ini"

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
  bool m_editingKey;
  mINI::INIFile *m_loaderIni;
  mINI::INIStructure m_ini;

  const char* keyToUnicode(u64 key);
  std::string keyToKeyChars(u64 key, bool overrideByDefault);
};

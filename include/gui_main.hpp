#pragma once

#include "gui.hpp"

#include <vector>
#include <unordered_map>

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

  const char* keyToUnicode(u64 key);
};

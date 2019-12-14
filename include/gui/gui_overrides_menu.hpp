#pragma once

#include "gui.hpp"
#include "override_key.hpp"

enum class OverrideButtonType {
  Album,
  Any_Title,
  Custom_Title,
  AddNew,
};


class GuiOverridesMenu : public Gui {
public:
  GuiOverridesMenu();
  ~GuiOverridesMenu();

  void update();
  void draw();
  void onInput(u32 kdown);
  void onTouch(touchPosition &touch);
  void onGesture(touchPosition &startPosition, touchPosition &endPosition);

  void addButton(OverrideButtonType type, const ProgramOverrideKey &key=ProgramOverrideKey{});
  void loadConfigFile();

private:
  ProgramOverrideKey m_overrides[8]{};
  ProgramOverrideKey m_anyAppOverride{};
  bool m_overrideAnyApp = false;
  u8 m_buttonCount = 0;
};

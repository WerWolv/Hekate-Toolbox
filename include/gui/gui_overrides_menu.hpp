#pragma once

#include "gui.hpp"
#include "override_key.hpp"

enum class OverrideButtonType {
  Album,
  Any_Title,
  Custom_Title,
  AddNew,
};

inline const char* buttonNames[] {
  "Album",
  "Custom title 1",
  "Custom title 2",
  "Custom title 3",
  "Custom title 4",
  "Custom title 5",
  "Custom title 6",
  "Custom title 7",
  "Any title",
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

  void addButton(OverrideButtonType buttonType, OverrideKeyType keyType = static_cast<OverrideKeyType>(0), const ProgramOverrideKey &key=ProgramOverrideKey{});
  void loadConfigFile();
  void removeFromList(OverrideKeyType keyEntry);

private:
  ProgramOverrideKey m_overrides[8]{};
  ProgramOverrideKey m_anyAppOverride{};
  bool m_overrideAnyApp = false;
  u8 m_buttonCount = 0;

  OverrideKeyType m_selectedConfig;
  std::vector<OverrideKeyType> m_addConfigs;
  std::vector<std::tuple<OverrideButtonType, OverrideKeyType>> m_buttons;
};

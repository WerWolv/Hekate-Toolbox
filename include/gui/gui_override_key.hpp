#pragma once

#include "gui.hpp"
#include "override_key.hpp"


class GuiOverrideKey : public Gui {
public:
  GuiOverrideKey();
  ~GuiOverrideKey();

  void update();
  void draw();
  void onInput(u32 kdown);
  void onTouch(touchPosition &touch);
  void onGesture(touchPosition &startPosition, touchPosition &endPosition);

  //the override_key option names as they are defined in the Atmosphere config
  const char* getOverrideKeyString(OverrideKeyType type);
  //the program_id option names as they are defined in the Atmosphere config
  const char* getOverrideProgramString(OverrideKeyType type);

private:
  ProgramOverrideKey m_override{};
  bool m_inputBlocked = false;
};

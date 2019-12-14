#pragma once
#include <string>
#include <switch.h>

struct OverrideKey {
  HidControllerKeys key;
  bool overrideByDefault;
  u64 programID;

  inline std::string ToString() { return KeyComboToString(*this); }

  static OverrideKey StringToKeyCombo(const std::string& keyString);
  static OverrideKey StringToKeyCombo(const char* keyString);
  static std::string KeyComboToString(const OverrideKey& keyCombo);
  static const char* KeyToUnicode(HidControllerKeys key);
};

struct ProgramOverrideKey {
  u64 programID;
  OverrideKey key;
};

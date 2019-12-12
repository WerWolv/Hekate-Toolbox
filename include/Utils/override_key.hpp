#pragma once
#include <string>
#include <switch.h>

class OverrideKey {
public:
  OverrideKey() {}
  OverrideKey(HidControllerKeys key, bool overrideByDefault) : key(key), overrideByDefault(overrideByDefault) {}

  inline std::string ToString() { return KeyComboToString(*this); }

  static OverrideKey StringToKeyCombo(const std::string& keyString);
  static OverrideKey StringToKeyCombo(const char* keyString);
  static std::string KeyComboToString(const OverrideKey& keyCombo);
  static const char* KeyToUnicode(HidControllerKeys key);

public:
  HidControllerKeys key;
  bool overrideByDefault;
};

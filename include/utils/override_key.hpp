#pragma once
#include <string>
#include <switch.h>

enum class OverrideKeyType {
  Override0 = 0,  //default when un-numbered
  Override1,
  Override2,
  Override3,
  Override4,
  Override5,
  Override6,
  Override7,
  AnyAppOverride,
  NUM_OVERRIDEKEY_TYPES,
};

#define OVERRIDEKEY_TYPES static_cast<int>(OverrideKeyType::NUM_OVERRIDEKEY_TYPES)

struct OverrideKey {
  HidControllerKeys key;
  bool overrideByDefault;
  u64 programID;

  inline std::string ToString() { return KeyComboToString(*this); }

  static OverrideKey StringToKeyCombo(const std::string& keyString);
  static OverrideKey StringToKeyCombo(const char* keyString);
  static std::string KeyComboToString(const OverrideKey& keyCombo);
  static const char* KeyToUnicode(HidControllerKeys key);

  //the override_key option names as they are defined in the Atmosphere config
  static const char* getOverrideKeyString(OverrideKeyType type);
  //the program_id option names as they are defined in the Atmosphere config
  static const char* getOverrideProgramString(OverrideKeyType type);
};

struct ProgramOverrideKey {
  u64 programID;
  OverrideKey key;
};

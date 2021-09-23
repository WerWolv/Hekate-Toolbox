#pragma once
#include <string>
#include <switch.h>

#define OVERRIDE_KEY "override_key_0"
#define PROGRAM_ID   "program_id_0"

enum class OverrideKeyType {
    Invalid = -1,
    Default = 0, // alias for Override0
    Override_1,
    Override_2,
    Override_3,
    Override_4,
    Override_5,
    Override_6,
    Override_7,
    Any_App_Override,
    Num_OverrideKey_Types,
};

struct OverrideKey {
    HidNpadButton key;
    bool overrideByDefault;
    u64 programID;

    inline std::string ToString() { return KeyComboToString(*this); }

    static OverrideKey StringToKeyCombo(const std::string &keyString);
    static OverrideKey StringToKeyCombo(const char *keyString);
    static std::string KeyComboToString(const OverrideKey &keyCombo);
    static const char *KeyToUnicode(HidNpadButton key);

    //the override_key option names as they are defined in the Atmosphere config
    static const char *getOverrideKeyString(OverrideKeyType type);
    //the program_id option names as they are defined in the Atmosphere config
    static const char *getOverrideProgramString(OverrideKeyType type);
};

struct ProgramOverrideKey {
    u64 programID;
    OverrideKey key;
};

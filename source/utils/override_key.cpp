#include "override_key.hpp"
#include <strings.h>

OverrideKey OverrideKey::StringToKeyCombo(const std::string &keyString) {
    return StringToKeyCombo(keyString.c_str());
}

OverrideKey OverrideKey::StringToKeyCombo(const char *keyString) {
    OverrideKey keyCombo;

    if (keyString[0] == '!') {
        keyCombo.overrideByDefault = true;
        ++keyString;
    } else {
        keyCombo.overrideByDefault = false;
    }

    if (strcasecmp(keyString, "A") == 0) {
        keyCombo.key = HidNpadButton_A;
    } else if (strcasecmp(keyString, "B") == 0) {
        keyCombo.key = HidNpadButton_B;
    } else if (strcasecmp(keyString, "X") == 0) {
        keyCombo.key = HidNpadButton_X;
    } else if (strcasecmp(keyString, "Y") == 0) {
        keyCombo.key = HidNpadButton_Y;
    } else if (strcasecmp(keyString, "LS") == 0) {
        keyCombo.key = HidNpadButton_StickL;
    } else if (strcasecmp(keyString, "RS") == 0) {
        keyCombo.key = HidNpadButton_StickR;
    } else if (strcasecmp(keyString, "L") == 0) {
        keyCombo.key = HidNpadButton_L;
    } else if (strcasecmp(keyString, "R") == 0) {
        keyCombo.key = HidNpadButton_R;
    } else if (strcasecmp(keyString, "ZL") == 0) {
        keyCombo.key = HidNpadButton_ZL;
    } else if (strcasecmp(keyString, "ZR") == 0) {
        keyCombo.key = HidNpadButton_ZR;
    } else if (strcasecmp(keyString, "PLUS") == 0) {
        keyCombo.key = HidNpadButton_Plus;
    } else if (strcasecmp(keyString, "MINUS") == 0) {
        keyCombo.key = HidNpadButton_Minus;
    } else if (strcasecmp(keyString, "DLEFT") == 0) {
        keyCombo.key = HidNpadButton_Left;
    } else if (strcasecmp(keyString, "DUP") == 0) {
        keyCombo.key = HidNpadButton_Up;
    } else if (strcasecmp(keyString, "DRIGHT") == 0) {
        keyCombo.key = HidNpadButton_Right;
    } else if (strcasecmp(keyString, "DDOWN") == 0) {
        keyCombo.key = HidNpadButton_Down;
    } else if (strcasecmp(keyString, "SL") == 0) {
        keyCombo.key = HidNpadButton_AnySL;
    } else if (strcasecmp(keyString, "SR") == 0) {
        keyCombo.key = HidNpadButton_AnySR;
    } else {
        keyCombo.key = static_cast<HidNpadButton>(0);
    }

    return keyCombo;
}

std::string OverrideKey::KeyComboToString(const OverrideKey &keyCombo) {
    std::string keyString;
    switch (keyCombo.key) {
        // clang-format off
    case HidNpadButton_A:       keyString = "A";        break;
    case HidNpadButton_B:       keyString = "B";        break;
    case HidNpadButton_X:       keyString = "X";        break;
    case HidNpadButton_Y:       keyString = "Y";        break;
    case HidNpadButton_StickL:  keyString = "LS";       break;
    case HidNpadButton_StickR:  keyString = "RS";       break;
    case HidNpadButton_L:       keyString = "L";        break;
    case HidNpadButton_R:       keyString = "R";        break;
    case HidNpadButton_ZL:      keyString = "ZL";       break;
    case HidNpadButton_ZR:      keyString = "ZR";       break;
    case HidNpadButton_Plus:    keyString = "PLUS";     break;
    case HidNpadButton_Minus:   keyString = "MINUS";    break;
    case HidNpadButton_Left:   keyString = "DLEFT";    break;
    case HidNpadButton_Up:     keyString = "DUP";      break;
    case HidNpadButton_Right:  keyString = "DRIGHT";   break;
    case HidNpadButton_Down:   keyString = "DDOWN";    break;
    case HidNpadButton_AnySL:      keyString = "SL";       break;
    case HidNpadButton_AnySR:      keyString = "SR";       break;
    default:          keyString = "";         break;
            // clang-format on
    }

    return (keyCombo.overrideByDefault ? "!" : "") + keyString;
}

const char *OverrideKey::KeyToUnicode(HidNpadButton key) {
    switch (key) {
        // clang-format off
    case HidNpadButton_A:       return "\uE0E0";
    case HidNpadButton_B:       return "\uE0E1";
    case HidNpadButton_X:       return "\uE0E2";
    case HidNpadButton_Y:       return "\uE0E3";
    case HidNpadButton_StickL:  return "\uE101";
    case HidNpadButton_StickR:  return "\uE102";
    case HidNpadButton_L:       return "\uE0E4";
    case HidNpadButton_R:       return "\uE0E5";
    case HidNpadButton_ZL:      return "\uE0E6";
    case HidNpadButton_ZR:      return "\uE0E7";
    case HidNpadButton_Plus:    return "\uE0EF";
    case HidNpadButton_Minus:   return "\uE0F0";
    case HidNpadButton_Left:   return "\uE0ED";
    case HidNpadButton_Up:     return "\uE0EB";
    case HidNpadButton_Right:  return "\uE0EE";
    case HidNpadButton_Down:   return "\uE0EC";
    case HidNpadButton_AnySL:      return "\uE0E8";
    case HidNpadButton_AnySR:      return "\uE0E9";
    default:          return "";
            // clang-format on
    }
}

const char *OverrideKey::getOverrideKeyString(OverrideKeyType type) {
    switch (type) {
        case OverrideKeyType::Any_App_Override:
            return "override_any_app_key";
        case OverrideKeyType::Default:
            return "override_key";
        case OverrideKeyType::Override_1:
            return "override_key_1";
        case OverrideKeyType::Override_2:
            return "override_key_2";
        case OverrideKeyType::Override_3:
            return "override_key_3";
        case OverrideKeyType::Override_4:
            return "override_key_4";
        case OverrideKeyType::Override_5:
            return "override_key_5";
        case OverrideKeyType::Override_6:
            return "override_key_6";
        case OverrideKeyType::Override_7:
            return "override_key_7";
        default:
            return "";
    }
}

const char *OverrideKey::getOverrideProgramString(OverrideKeyType type) {
    switch (type) {
        case OverrideKeyType::Default:
            return "program_id";
        case OverrideKeyType::Override_1:
            return "program_id_1";
        case OverrideKeyType::Override_2:
            return "program_id_2";
        case OverrideKeyType::Override_3:
            return "program_id_3";
        case OverrideKeyType::Override_4:
            return "program_id_4";
        case OverrideKeyType::Override_5:
            return "program_id_5";
        case OverrideKeyType::Override_6:
            return "program_id_6";
        case OverrideKeyType::Override_7:
            return "program_id_7";
        default:
            return "";
    }
}
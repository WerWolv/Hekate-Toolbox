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
        keyCombo.key = KEY_A;
    } else if (strcasecmp(keyString, "B") == 0) {
        keyCombo.key = KEY_B;
    } else if (strcasecmp(keyString, "X") == 0) {
        keyCombo.key = KEY_X;
    } else if (strcasecmp(keyString, "Y") == 0) {
        keyCombo.key = KEY_Y;
    } else if (strcasecmp(keyString, "LS") == 0) {
        keyCombo.key = KEY_LSTICK;
    } else if (strcasecmp(keyString, "RS") == 0) {
        keyCombo.key = KEY_RSTICK;
    } else if (strcasecmp(keyString, "L") == 0) {
        keyCombo.key = KEY_L;
    } else if (strcasecmp(keyString, "R") == 0) {
        keyCombo.key = KEY_R;
    } else if (strcasecmp(keyString, "ZL") == 0) {
        keyCombo.key = KEY_ZL;
    } else if (strcasecmp(keyString, "ZR") == 0) {
        keyCombo.key = KEY_ZR;
    } else if (strcasecmp(keyString, "PLUS") == 0) {
        keyCombo.key = KEY_PLUS;
    } else if (strcasecmp(keyString, "MINUS") == 0) {
        keyCombo.key = KEY_MINUS;
    } else if (strcasecmp(keyString, "DLEFT") == 0) {
        keyCombo.key = KEY_DLEFT;
    } else if (strcasecmp(keyString, "DUP") == 0) {
        keyCombo.key = KEY_DUP;
    } else if (strcasecmp(keyString, "DRIGHT") == 0) {
        keyCombo.key = KEY_DRIGHT;
    } else if (strcasecmp(keyString, "DDOWN") == 0) {
        keyCombo.key = KEY_DDOWN;
    } else if (strcasecmp(keyString, "SL") == 0) {
        keyCombo.key = KEY_SL;
    } else if (strcasecmp(keyString, "SR") == 0) {
        keyCombo.key = KEY_SR;
    } else {
        keyCombo.key = static_cast<HidControllerKeys>(0);
    }

    return keyCombo;
}

std::string OverrideKey::KeyComboToString(const OverrideKey &keyCombo) {
    std::string keyString;
    switch (keyCombo.key) {
        // clang-format off
    case KEY_A:       keyString = "A";        break;
    case KEY_B:       keyString = "B";        break;
    case KEY_X:       keyString = "X";        break;
    case KEY_Y:       keyString = "Y";        break;
    case KEY_LSTICK:  keyString = "LS";       break;
    case KEY_RSTICK:  keyString = "RS";       break;
    case KEY_L:       keyString = "L";        break;
    case KEY_R:       keyString = "R";        break;
    case KEY_ZL:      keyString = "ZL";       break;
    case KEY_ZR:      keyString = "ZR";       break;
    case KEY_PLUS:    keyString = "PLUS";     break;
    case KEY_MINUS:   keyString = "MINUS";    break;
    case KEY_DLEFT:   keyString = "DLEFT";    break;
    case KEY_DUP:     keyString = "DUP";      break;
    case KEY_DRIGHT:  keyString = "DRIGHT";   break;
    case KEY_DDOWN:   keyString = "DDOWN";    break;
    case KEY_SL:      keyString = "SL";       break;
    case KEY_SR:      keyString = "SR";       break;
    default:          keyString = "";         break;
            // clang-format on
    }

    return (keyCombo.overrideByDefault ? "!" : "") + keyString;
}

const char *OverrideKey::KeyToUnicode(HidControllerKeys key) {
    switch (key) {
        // clang-format off
    case KEY_A:       return "\uE0E0";
    case KEY_B:       return "\uE0E1";
    case KEY_X:       return "\uE0E2";
    case KEY_Y:       return "\uE0E3";
    case KEY_LSTICK:  return "\uE101";
    case KEY_RSTICK:  return "\uE102";
    case KEY_L:       return "\uE0E4";
    case KEY_R:       return "\uE0E5";
    case KEY_ZL:      return "\uE0E6";
    case KEY_ZR:      return "\uE0E7";
    case KEY_PLUS:    return "\uE0EF";
    case KEY_MINUS:   return "\uE0F0";
    case KEY_DLEFT:   return "\uE0ED";
    case KEY_DUP:     return "\uE0EB";
    case KEY_DRIGHT:  return "\uE0EE";
    case KEY_DDOWN:   return "\uE0EC";
    case KEY_SL:      return "\uE0E8";
    case KEY_SR:      return "\uE0E9";
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
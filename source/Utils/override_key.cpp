#include "override_key.hpp"
#include <strings.h>

OverrideKey OverrideKey::StringToKeyCombo(const std::string& keyString) {
  return StringToKeyCombo(keyString.c_str());
}

OverrideKey OverrideKey::StringToKeyCombo(const char* keyString) {
  OverrideKey keyCombo;

  if (keyString[0] == '!') {
    keyCombo.overrideByDefault = true;
    ++keyString;
  }
  else {
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
  }

  return keyCombo;
}

std::string OverrideKey::KeyComboToString(const OverrideKey& keyCombo) {
  std::string keyString;
  switch(keyCombo.key) {
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
    default:          break;
  }

  return (keyCombo.overrideByDefault ? "!" : "") + keyString;
}

const char* OverrideKey::KeyToUnicode(HidControllerKeys key) {
  switch(key) {
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
  }
}
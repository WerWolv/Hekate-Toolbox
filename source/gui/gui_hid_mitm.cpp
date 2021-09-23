#include "gui_hid_mitm.hpp"
#include "button.hpp"

extern "C" {
#include "hid_extra.h"
}

#include <stdio.h>
#include <dirent.h>
#include <sstream>
#include <fstream>
#include <utility>
#include <math.h>
#include "threads.hpp"

#include "switch_bin.h"

const char *const keyNames[16] = {"KEY_A", "KEY_B", "KEY_X", "KEY_Y", "KEY_LSTICK", "KEY_RSTICK", "KEY_L", "KEY_R", "KEY_ZL", "KEY_ZR", "KEY_PLUS", "KEY_MINUS", "KEY_DLEFT", "KEY_DUP", "KEY_DRIGHT", "KEY_DDOWN"};

GuiHIDMitm::GuiHIDMitm() : Gui() {
    hidExtraPause();
    //TODO: fix the whole iniparser thing if hid-mitm ever comes back

    m_configFile = simpleIniParser::Ini::parseFile(HID_MITM_INI);

    if (m_configFile != nullptr && m_configFile->findSection("player1") != nullptr) {
        for (u8 i = 0; i < m_configFile->findSection("player1")->options.size(); i++) {
            m_hidConfig[m_configFile->findSection("player1")->options[i]->key] = m_configFile->findSection("player1")->options[i]->value;
        }
    }

    auto configButton = new Button();
    configButton->position = {400, 400};
    configButton->volume = {Gui::g_framebuffer_width - 800, 80};
    configButton->drawAction = [&](Gui *gui, u16 x, u16 y, bool *isActivated) {
        gui->drawTextAligned(font20, Gui::g_framebuffer_width / 2, y + 50, currTheme.textColor, "Touch to save config", ALIGNED_CENTER);
    };
    configButton->inputAction = [&](u32 kdown, bool *isActivated) {
        if (kdown & HidNpadButton_A) {
            Gui::g_nextGui = GUI_SM_SELECT;

            for (u16 i = 0; i < m_hidConfig.size(); i++)
                m_configFile->findSection("player1")->options[i]->value = m_hidConfig[m_configFile->findSection("player1")->options[i]->key];
        }
    };
    add(configButton);
    endInit();
}

GuiHIDMitm::~GuiHIDMitm() {
    m_configFile->writeToFile(HID_MITM_INI);
    delete m_configFile;
    hidExtraReloadConfig();
}

static std::string configToButtonName(std::string configName) {
    // clang-format off
  if (configName == "KEY_A") return "\uE0E0";
  else if (configName == "KEY_B") return "\uE0E1";
  else if (configName == "KEY_X") return "\uE0E2";
  else if (configName == "KEY_Y") return "\uE0E3";
  else if (configName == "KEY_LSTICK") return "\uE104";
  else if (configName == "KEY_RSTICK") return "\uE105";
  else if (configName == "KEY_L") return "\uE0E4";
  else if (configName == "KEY_R") return "\uE0E5";
  else if (configName == "KEY_ZL") return "\uE0E6";
  else if (configName == "KEY_ZR") return "\uE0E7";
  else if (configName == "KEY_PLUS") return "\uE0EF";
  else if (configName == "KEY_MINUS") return "\uE0F0";
  else if (configName == "KEY_DLEFT") return "\uE0ED";
  else if (configName == "KEY_DUP") return "\uE0EB";
  else if (configName == "KEY_DRIGHT") return "\uE0EE";
  else if (configName == "KEY_DDOWN") return "\uE0EC";
  else return "\uE0E0";
    // clang-format on
}

void GuiHIDMitm::draw() {
    Gui::beginDraw();
    Gui::drawRectangle(0, 0, Gui::g_framebuffer_width, Gui::g_framebuffer_height, currTheme.backgroundColor);
    Gui::drawRectangle((u32)((Gui::g_framebuffer_width - 1220) / 2), 87, 1220, 1, currTheme.textColor);
    Gui::drawRectangle((u32)((Gui::g_framebuffer_width - 1220) / 2), Gui::g_framebuffer_height - 73, 1220, 1, currTheme.textColor);
    Gui::drawTextAligned(fontIcons, 70, 68, currTheme.textColor, "\uE130", ALIGNED_LEFT);
    Gui::drawTextAligned(font24, 70, 58, currTheme.textColor, "        Hekate Toolbox", ALIGNED_LEFT);
    Gui::drawTextAligned(font20, Gui::g_framebuffer_width - 50, Gui::g_framebuffer_height - 25, currTheme.textColor, "\uE122 Remap", ALIGNED_RIGHT);

    Gui::drawImage(Gui::g_framebuffer_width / 2 - 500, Gui::g_framebuffer_height / 2 - 220, 1001, 441, switch_bin, IMAGE_MODE_RGBA32);

    Gui::drawTextAligned(font20, Gui::g_framebuffer_width / 2, 250, COLOR_WHITE, "This menu remaps one joycon button to another. \n First press the button you want to remap and \n then the button to remap it to.", ALIGNED_CENTER);

    // clang-format off
  Gui::drawTextAligned(font20, 1100, 264, m_selectedButton ==  0 ? currTheme.selectedColor : COLOR_WHITE,         configToButtonName(m_hidConfig["KEY_A"]).c_str(), ALIGNED_CENTER);
  Gui::drawTextAligned(font20, 1066, 297, m_selectedButton ==  1 ? currTheme.selectedColor : COLOR_WHITE,         configToButtonName(m_hidConfig["KEY_B"]).c_str(), ALIGNED_CENTER);
  Gui::drawTextAligned(font20, 1066, 228, m_selectedButton ==  2 ? currTheme.selectedColor : COLOR_WHITE,         configToButtonName(m_hidConfig["KEY_X"]).c_str(), ALIGNED_CENTER);
  Gui::drawTextAligned(font20, 1031, 262, m_selectedButton ==  3 ? currTheme.selectedColor : COLOR_WHITE,         configToButtonName(m_hidConfig["KEY_Y"]).c_str(), ALIGNED_CENTER);
  Gui::drawTextAligned(font20, 218, 264,  m_selectedButton ==  4 ? currTheme.selectedColor : COLOR_WHITE,         configToButtonName(m_hidConfig["KEY_LSTICK"]).c_str(), ALIGNED_CENTER);
  Gui::drawTextAligned(font20, 1063, 381, m_selectedButton ==  5 ? currTheme.selectedColor : COLOR_WHITE,         configToButtonName(m_hidConfig["KEY_RSTICK"]).c_str(), ALIGNED_CENTER);
  Gui::drawTextAligned(font20, 246, 135,  m_selectedButton ==  6 ? currTheme.selectedColor : currTheme.textColor, configToButtonName(m_hidConfig["KEY_L"]).c_str(), ALIGNED_CENTER);
  Gui::drawTextAligned(font20, 1038, 135, m_selectedButton ==  7 ? currTheme.selectedColor : currTheme.textColor, configToButtonName(m_hidConfig["KEY_R"]).c_str(), ALIGNED_CENTER);
  Gui::drawTextAligned(font20, 153, 163,  m_selectedButton ==  8 ? currTheme.selectedColor : currTheme.textColor, configToButtonName(m_hidConfig["KEY_ZL"]).c_str(), ALIGNED_CENTER);
  Gui::drawTextAligned(font20, 1132, 163, m_selectedButton ==  9 ? currTheme.selectedColor : currTheme.textColor, configToButtonName(m_hidConfig["KEY_ZR"]).c_str(), ALIGNED_CENTER);
  Gui::drawTextAligned(font20, 1023, 189, m_selectedButton == 10 ? currTheme.selectedColor : COLOR_WHITE,         configToButtonName(m_hidConfig["KEY_PLUS"]).c_str(), ALIGNED_CENTER);
  Gui::drawTextAligned(font20, 259, 186,  m_selectedButton == 11 ? currTheme.selectedColor : COLOR_WHITE,         configToButtonName(m_hidConfig["KEY_MINUS"]).c_str(), ALIGNED_CENTER);
  Gui::drawTextAligned(font20, 184, 379,  m_selectedButton == 12 ? currTheme.selectedColor : COLOR_WHITE,         configToButtonName(m_hidConfig["KEY_DLEFT"]).c_str(), ALIGNED_CENTER);
  Gui::drawTextAligned(font20, 218, 344,  m_selectedButton == 13 ? currTheme.selectedColor : COLOR_WHITE,         configToButtonName(m_hidConfig["KEY_DUP"]).c_str(), ALIGNED_CENTER);
  Gui::drawTextAligned(font20, 253, 379,  m_selectedButton == 14 ? currTheme.selectedColor : COLOR_WHITE,         configToButtonName(m_hidConfig["KEY_DRIGHT"]).c_str(), ALIGNED_CENTER);
  Gui::drawTextAligned(font20, 218, 414,  m_selectedButton == 15 ? currTheme.selectedColor : COLOR_WHITE,         configToButtonName(m_hidConfig["KEY_DDOWN"]).c_str(), ALIGNED_CENTER);
    // clang-format on
    drawButtons();
    Gui::endDraw();
}

void GuiHIDMitm::onInput(u32 kdown) {
    if (m_selectedButton == -1) {
        if (kdown <= HidNpadButton_Down)
            m_selectedButton = log2(kdown);
    } else {
        if (kdown <= HidNpadButton_Down) {
            m_hidConfig[keyNames[m_selectedButton]] = keyNames[static_cast<u32>(log2(kdown))];
            m_selectedButton = -1;
        }
    }
}
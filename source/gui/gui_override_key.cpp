#include "gui_override_key.hpp"
#include "button.hpp"
#include "utils.hpp"
#include "SimpleIniParser.hpp"

#include "list_selector.hpp"
#include "override_key.hpp"
#include "titleinfo.hpp"
#include "gui_title_list.hpp"

GuiOverrideKey::GuiOverrideKey() : Gui() {
    m_scrollBlocked = true;
    loadConfigFile();

    //0
    auto keySelectButton = Button();
    keySelectButton.position = {640, 200};
    keySelectButton.volume = {380, 200};
    keySelectButton.activatable = true;
    keySelectButton.adjacentButton[ADJ_DOWN] = 1;
    keySelectButton.adjacentButton[ADJ_LEFT] = 2;
    keySelectButton.drawAction = [&](Gui *gui, u16 x, u16 y, bool *isActivated) {
        if (m_inputBlocked)
            gui->drawTextAligned(font20, x + 190, y + 100, currTheme.textColor, "Press any key...", ALIGNED_CENTER);
        else if (m_override.key.key == 0)
            gui->drawTextAligned(font24, x + 190, y + 100, currTheme.unselectedColor, "No key selected", ALIGNED_CENTER);
        else
            gui->drawTextAligned(fontHuge, x + 190, y + 136, currTheme.textColor, OverrideKey::KeyToUnicode(m_override.key.key), ALIGNED_CENTER);
        gui->drawTextAligned(font14, x + 190, y + 185, currTheme.textColor, "Override key", ALIGNED_CENTER);
    };
    keySelectButton.inputAction = [&](u64 kdown, bool *isActivated) {
        if (*isActivated) {

            // This is supposed to clear the key display, and block exit until a button is pressed.
            // For some reason, it doesn't work
            m_override.key.key = static_cast<HidControllerKeys>(0);
            m_inputBlocked = true;
            Gui::g_exitBlocked = true;
            if (kdown && !(kdown & (kdown - 1)) && (kdown <= KEY_DDOWN || kdown >= KEY_SL) && kdown != KEY_TOUCH) {
                m_override.key.key = static_cast<HidControllerKeys>(kdown);
                //Find or create a loader ini file with set override_key values, and write the result to the file.
                simpleIniParser::Ini *ini = parseOrCreateFileFixed(LOADER_INI);
                auto keyValue = m_override.key.ToString();
                ini->findOrCreateSection(HBL_CONFIG, true, simpleIniParser::IniSectionType::Section)->findOrCreateFirstOption(OverrideKey::getOverrideKeyString(g_keyType), "")->value = keyValue;

                ini->writeToFile(LOADER_INI);
                *isActivated = false;
                m_inputBlocked = false;
                Gui::g_exitBlocked = false;

                delete ini;
            }
        }
    };
    add(keySelectButton);

    //1
    auto comboPressedButton = Button();
    comboPressedButton.position = {640, 420};
    comboPressedButton.volume = {380, 80};
    comboPressedButton.adjacentButton[ADJ_UP] = 0;
    comboPressedButton.adjacentButton[ADJ_DOWN] = g_keyType == OverrideKeyType::Any_App_Override ? 3 : -1;
    comboPressedButton.adjacentButton[ADJ_LEFT] = 2;
    comboPressedButton.drawAction = [&](Gui *gui, u16 x, u16 y, bool *isActivated) {
        gui->drawTextAligned(font20, x + 20, y + 50, currTheme.textColor, "Key must be:", ALIGNED_LEFT);
        gui->drawTextAligned(font20, x + 360, y + 50, currTheme.selectedColor, m_override.key.overrideByDefault ? "Unpressed" : "Pressed", ALIGNED_RIGHT);
    };
    comboPressedButton.inputAction = [&](u32 kdown, bool *isActivated) {
        if (kdown & KEY_A) {
            m_override.key.overrideByDefault = !m_override.key.overrideByDefault;
            if (m_override.key.key == static_cast<HidControllerKeys>(0))
                return;

            //Find or create a loader ini file with set override_key values, and write the result to the file.
            simpleIniParser::Ini *ini = parseOrCreateFileFixed(LOADER_INI);
            auto keyValue = m_override.key.ToString();
            ini->findOrCreateSection(HBL_CONFIG, true, simpleIniParser::IniSectionType::Section)->findOrCreateFirstOption(OverrideKey::getOverrideKeyString(g_keyType), "")->value = keyValue;

            ini->writeToFile(LOADER_INI);
            delete ini;
        }
    };
    add(comboPressedButton);

    switch (g_keyType) {
        case OverrideKeyType::Any_App_Override: {
            //2
            auto anyTitleIconButton = Button();
            anyTitleIconButton.position = {220, 200};
            anyTitleIconButton.volume = {300, 300};
            anyTitleIconButton.adjacentButton[ADJ_RIGHT] = 0;
            anyTitleIconButton.drawAction = [&](Gui *gui, u16 x, u16 y, bool *isActivated) {
                gui->drawTextAligned(fontHuge, x + 150, y + 190, currTheme.textColor, "\uE135", ALIGNED_CENTER);
                gui->drawTextAligned(font24, x, y - 60, currTheme.textColor, "Override when entering:", ALIGNED_LEFT);
                gui->drawTextAligned(font24, x, y - 20, currTheme.textColor, "Any title", ALIGNED_LEFT);
            };
            anyTitleIconButton.usableCondition = []() -> bool { return false; };
            add(anyTitleIconButton);

            //3
            auto overrideEnabledButton = Button();
            overrideEnabledButton.position = {640, 520};
            overrideEnabledButton.volume = {380, 80};
            overrideEnabledButton.adjacentButton[ADJ_UP] = 1;
            overrideEnabledButton.adjacentButton[ADJ_LEFT] = 2;
            overrideEnabledButton.drawAction = [&](Gui *gui, u16 x, u16 y, bool *isActivated) {
                gui->drawTextAligned(font20, x + 20, y + 50, currTheme.textColor, "Enabled", ALIGNED_LEFT);
                gui->drawTextAligned(font20, x + 360, y + 50, m_overrideAnyApp ? currTheme.selectedColor : currTheme.unselectedColor, m_overrideAnyApp ? "On" : "Off", ALIGNED_RIGHT);
            };
            overrideEnabledButton.inputAction = [&](u64 kdown, bool *isActivated) {
                if (kdown & KEY_A) {
                    m_overrideAnyApp = !m_overrideAnyApp;

                    //Find or create a loader ini file with set override_key values, and write the result to the file.
                    simpleIniParser::Ini *ini = parseOrCreateFileFixed(LOADER_INI);
                    ini->findOrCreateSection(HBL_CONFIG, true, simpleIniParser::IniSectionType::Section)->findOrCreateFirstOption("override_any_app", "")->value = m_overrideAnyApp ? "true" : "false";

                    ini->writeToFile(LOADER_INI);
                    delete ini;
                }
            };
            add(overrideEnabledButton);
            break;
        }
        default: {
            //2
            auto appIconButton = Button();
            appIconButton.position = {220, 200};
            appIconButton.volume = {300, 300};
            appIconButton.adjacentButton[ADJ_DOWN] = 1;
            appIconButton.adjacentButton[ADJ_RIGHT] = 0;
            appIconButton.drawAction = [&, title{DumpTitle(m_override.programID)}](Gui *gui, u16 x, u16 y, bool *isActivated) {
                gui->drawTextAligned(font24, x, y - 60, currTheme.textColor, "Override when entering:", ALIGNED_LEFT);
                if (title.get() != nullptr && title->application_id != 0) {

                    auto appletName = GetAppletName(title->application_id);
                    if (appletName == nullptr) {
                        appletName = title->name;
                    } else {
                        gui->drawTextAligned(font20, Gui::g_framebuffer_width / 2, y + 350, currTheme.activatedColor, "Opening hbmenu through an applet may not leave enough memory for homebrew applications. \n It's recommended to open hbmenu from a game to gain full RAM access.", ALIGNED_CENTER);
                    }

                    gui->drawTextAligned(font24, x, y - 20, currTheme.textColor, appletName, ALIGNED_LEFT);

                    if (title->icon.get() != nullptr)
                        gui->drawImage(x + 22, y + 22, 256, 256, title->icon.get(), ImageMode::IMAGE_MODE_RGBA32);
                    else
                        gui->drawTextAligned(fontHuge, x + 150, y + 186, GetAppletColor(title->application_id), GetAppletIcon(title->application_id), ALIGNED_CENTER);
                } else {
                    gui->drawTextAligned(fontHuge, x + 150, y + 186, currTheme.unselectedColor, "\uE06B", ALIGNED_CENTER);
                    gui->drawTextAligned(font24, x + 150, y + 280, currTheme.unselectedColor, "No title selected", ALIGNED_CENTER);
                }
            };
            appIconButton.inputAction = [&](u64 kdown, bool *isActivated) {
                if (kdown & KEY_A) {
                    GuiTitleList::selectedAppID = m_override.programID;
                    Gui::g_nextGui = GUI_TITLE_LIST;
                }
            };
            add(appIconButton);
            break;
        }
    }
    selectButton(selection);
    endInit();
}

GuiOverrideKey::~GuiOverrideKey() {
    selection = getSelectedButtonIndex();
}

void GuiOverrideKey::draw() {
    Gui::beginDraw();
    Gui::drawRectangle(0, 0, Gui::g_framebuffer_width, Gui::g_framebuffer_height, currTheme.backgroundColor);
    Gui::drawRectangle((u32)((Gui::g_framebuffer_width - 1220) / 2), 87, 1220, 1, currTheme.textColor);
    Gui::drawRectangle((u32)((Gui::g_framebuffer_width - 1220) / 2), Gui::g_framebuffer_height - 73, 1220, 1, currTheme.textColor);
    Gui::drawTextAligned(fontIcons, 70, 68, currTheme.textColor, "\uE130", ALIGNED_LEFT);
    Gui::drawTextAligned(font24, 70, 58, currTheme.textColor, "        Application override settings", ALIGNED_LEFT);
    Gui::drawTextAligned(font20, Gui::g_framebuffer_width - 50, Gui::g_framebuffer_height - 25, currTheme.textColor, "\uE0E1 Back     \uE0E0 OK", ALIGNED_RIGHT);

    drawButtons();
    Gui::endDraw();
}

void GuiOverrideKey::onInput(u32 kdown) {
    if (!m_inputBlocked && kdown & KEY_B)
        Gui::g_nextGui = GUI_OVERRIDES_MENU;

    inputButtons(kdown);
}

void GuiOverrideKey::loadConfigFile() {
    // Get the override keys, if any exist
    auto ini = parseOrCreateFileFixed(LOADER_INI);
    auto iniSection = ini->findOrCreateSection(HBL_CONFIG, true, simpleIniParser::IniSectionType::Section);

    // Get the override key and program for un-numbered override
    // TODO: this may be removed in a future atmosphere release
    if (g_keyType == OverrideKeyType::Default) {
        auto option = iniSection->findFirstOption(OVERRIDE_KEY);
        if (option != nullptr)
            m_override.key = OverrideKey::StringToKeyCombo(option->value);
        else
            m_override.key = OverrideKey::StringToKeyCombo("!R");

        option = iniSection->findFirstOption(PROGRAM_ID);
        if (option != nullptr)
            m_override.programID = strtoul(option->value.c_str(), nullptr, 16);
        else
            m_override.programID = AppletID::AppletPhotoViewer;
    }

    auto option = iniSection->findFirstOption(OverrideKey::getOverrideKeyString(g_keyType));
    if (option != nullptr)
        m_override.key = OverrideKey::StringToKeyCombo(option->value);
    else if (g_keyType == OverrideKeyType::Any_App_Override)
        m_override.key = OverrideKey::StringToKeyCombo("R");

    option = iniSection->findFirstOption(OverrideKey::getOverrideProgramString(g_keyType));
    if (option != nullptr)
        m_override.programID = strtoul(option->value.c_str(), nullptr, 16);

    //m_override.key = OverrideKey::StringToKeyCombo(iniSection->findOrCreateFirstOption(OverrideKey::getOverrideKeyString(g_keyType), "")->value);
    //m_override.programID = strtoul(iniSection->findOrCreateFirstOption(OverrideKey::getOverrideProgramString(g_keyType), "")->value.c_str(), nullptr, 16);
    option = iniSection->findFirstOption("override_any_app");
    if (option != nullptr)
        m_overrideAnyApp = (option->value == "true") || (option->value == "1");
    else
        m_overrideAnyApp = true;
    delete ini;
}
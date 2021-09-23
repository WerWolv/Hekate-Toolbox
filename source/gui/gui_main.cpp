#include "gui_main.hpp"
#include "button.hpp"

#include <sstream>

#include "utils.hpp"
#include "SimpleIniParser.hpp"

#include "list_selector.hpp"
#include "override_key.hpp"
#include "gui_hekate.hpp"

extern "C" {
#include "pm_dmnt.h"
}

extern bool g_exitApplet;
bool exitDisabled = false;

static std::vector<std::string> autobootNames;
static u16 currAutoBootEntryIndex;

GuiMain::GuiMain() : Gui() {
    {
        // Get the override keys, if any exist
        simpleIniParser::Ini *ini = parseOrCreateFileFixed(LOADER_INI);
        m_albumOverrideKey = OverrideKey::StringToKeyCombo(ini->findOrCreateSection(HBL_CONFIG, true, simpleIniParser::IniSectionType::Section)->findOrCreateFirstOption(OVERRIDE_KEY, "!R")->value);
        delete ini;
    }

    getBootConfigs(m_autoBootConfigs, currAutoBootEntryIndex);
    m_currAutoBootConfig = m_autoBootConfigs[currAutoBootEntryIndex];

    //0
    auto overrideKeysMenuButton = new Button();
    overrideKeysMenuButton->position = {150, 230};
    overrideKeysMenuButton->volume = {200, 200};
    overrideKeysMenuButton->adjacentButton[ADJ_DOWN] = 2;
    overrideKeysMenuButton->adjacentButton[ADJ_RIGHT] = 1;
    overrideKeysMenuButton->drawAction = [&](Gui *gui, u16 x, u16 y, bool *isActivated) {
        gui->drawTextAligned(fontHuge, x + 100, y + 100, currTheme.textColor, "...", ALIGNED_CENTER);
        gui->drawTextAligned(font14, x + 100, y + 185, currTheme.textColor, "Change override keys", ALIGNED_CENTER);
    };
    overrideKeysMenuButton->inputAction = [&](u64 kdown, bool *isActivated) {
        if (kdown & HidNpadButton_A)
            Gui::g_nextGui = GUI_OVERRIDES_MENU;
    };
    add(overrideKeysMenuButton);

    //1
    auto autobootSelectorButton = new Button();
    autobootSelectorButton->position = {370, 240};
    autobootSelectorButton->volume = {700, 80};
    autobootSelectorButton->adjacentButton[ADJ_DOWN] = 2;
    autobootSelectorButton->adjacentButton[ADJ_LEFT] = 0;
    autobootSelectorButton->drawAction = [&](Gui *gui, u16 x, u16 y, bool *isActivated) {
        gui->drawTextAligned(font20, x + 37, y + 50, currTheme.textColor, "Hekate autoboot profile", ALIGNED_LEFT);

        std::string autoBootName = m_currAutoBootConfig.name;

        if (autoBootName.length() >= 25) {
            autoBootName = autoBootName.substr(0, 24);
            autoBootName += "...";
        }

        gui->drawTextAligned(font20, x + 660, y + 50, currTheme.selectedColor, autoBootName.c_str(), ALIGNED_RIGHT);
    };
    autobootSelectorButton->inputAction = [&](u32 kdown, bool *isActivated) {
        if (kdown & HidNpadButton_A) {
            autobootNames.clear();

            for (auto const &autoBootEntry : m_autoBootConfigs)
                autobootNames.push_back(autoBootEntry.name);

            (new ListSelector("Hekate autoboot profile", "\uE0E1 Back     \uE0E0 OK", autobootNames, currAutoBootEntryIndex))
                ->setInputAction([&](u32 k, u16 selectedItem) {
                    if (k & HidNpadButton_A) {
                        auto hekateIni = parseOrCreateFileFixed(HEKATE_INI);

                        currAutoBootEntryIndex = selectedItem;
                        m_currAutoBootConfig = m_autoBootConfigs[selectedItem];

                        auto configSection = hekateIni->findOrCreateSection("config");

                        auto ini_autoboot = configSection->findOrCreateFirstOption("autoboot", "0");
                        auto ini_autoboot_list = configSection->findOrCreateFirstOption("autoboot_list", "0");

                        ini_autoboot->value = std::to_string(m_currAutoBootConfig.id);
                        ini_autoboot_list->value = std::to_string(m_currAutoBootConfig.autoBootList);

                        hekateIni->writeToFile(HEKATE_INI);
                        delete hekateIni;
                        Gui::g_currListSelector->hide();
                    }
                })
                ->show();
        }
    };
    add(autobootSelectorButton);

    //2
    auto backgroundServicesButton = new Button();
    backgroundServicesButton->position = {370, 340};
    backgroundServicesButton->volume = {340, 80};
    backgroundServicesButton->adjacentButton[ADJ_UP] = 1;
    backgroundServicesButton->adjacentButton[ADJ_LEFT] = 0;
    backgroundServicesButton->adjacentButton[ADJ_RIGHT] = 3;
    backgroundServicesButton->drawAction = [&](Gui *gui, u16 x, u16 y, bool *isActivated) {
        gui->drawRectangled(x, y, 340, 80, currTheme.submenuButtonColor);
        gui->drawTextAligned(font20, x + 37, y + 50, currTheme.textColor, "Background services", ALIGNED_LEFT);
    };
    backgroundServicesButton->inputAction = [&](u32 kdown, bool *isActivated) {
        if (kdown & HidNpadButton_A)
            Gui::g_nextGui = GUI_SM_SELECT;
    };
    add(backgroundServicesButton);

    //3
    auto rebootButton = new Button();
    rebootButton->position = {735, 340};
    rebootButton->volume = {335, 80};
    rebootButton->adjacentButton[ADJ_UP] = 1;
    rebootButton->adjacentButton[ADJ_LEFT] = 2;
    rebootButton->drawAction = [&](Gui *gui, u16 x, u16 y, bool *isActivated) {
        gui->drawRectangled(x, y, 335, 80, currTheme.submenuButtonColor);
        gui->drawTextAligned(font20, x + 55, y + 50, currTheme.textColor, "Reboot to Hekate", ALIGNED_LEFT);
    };
    rebootButton->inputAction = [&](u32 kdown, bool *isActivated) {
        if (kdown & HidNpadButton_A) {
            Gui::g_nextGui = GUI_HEKATE;
            GuiHekate::m_currRebootConfig = m_currAutoBootConfig;
        }
    };
    add(rebootButton);

    selectButton(selection);
    endInit();
}

GuiMain::~GuiMain() {
    selection = this->getSelectedButtonIndex();
}

void GuiMain::draw() {
    Gui::beginDraw();
    Gui::drawRectangle(0, 0, Gui::g_framebuffer_width, Gui::g_framebuffer_height, currTheme.backgroundColor);
    Gui::drawRectangle((u32)((Gui::g_framebuffer_width - 1220) / 2), 87, 1220, 1, currTheme.textColor);
    Gui::drawRectangle((u32)((Gui::g_framebuffer_width - 1220) / 2), Gui::g_framebuffer_height - 73, 1220, 1, currTheme.textColor);
    Gui::drawTextAligned(fontIcons, 70, 68, currTheme.textColor, "\uE130", ALIGNED_LEFT);
    Gui::drawTextAligned(font24, 70, 58, currTheme.textColor, "        Hekate Toolbox", ALIGNED_LEFT);
    Gui::drawTextAligned(font20, Gui::g_framebuffer_width - 50, Gui::g_framebuffer_height - 25, currTheme.textColor, "\uE0E1 Back     \uE0E0 OK", ALIGNED_RIGHT);
    Gui::drawTextAligned(font24, Gui::g_framebuffer_width / 2, Gui::g_framebuffer_height - 130, currTheme.textColor, "Press \uE044 to save and return back to the home menu", ALIGNED_CENTER);

    drawButtons();
    Gui::endDraw();
}

void GuiMain::onInput(u32 kdown) {
    if (inputButtons(kdown)) return;

    if (kdown & HidNpadButton_B && !exitDisabled)
        g_exitApplet = true;
}
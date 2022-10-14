#include "gui_hekate.hpp"
#include "button.hpp"

#include <stdio.h>
#include <dirent.h>
#include <sstream>
#include <fstream>
#include <utility>
#include "SimpleIniParser.hpp"

#include "list_selector.hpp"
#include "ams_bpc.h"

#define IRAM_PAYLOAD_MAX_SIZE 0x24000
static u8 g_reboot_payload[IRAM_PAYLOAD_MAX_SIZE];

enum NyxUMSType {
    NYX_UMS_SD_CARD = 0,
    NYX_UMS_EMMC_BOOT0,
    NYX_UMS_EMMC_BOOT1,
    NYX_UMS_EMMC_GPP,
    NYX_UMS_EMUMMC_BOOT0,
    NYX_UMS_EMUMMC_BOOT1,
    NYX_UMS_EMUMMC_GPP
};


static void reboot_to_payload(void) {
    Result rc = amsBpcSetRebootPayload(g_reboot_payload, IRAM_PAYLOAD_MAX_SIZE);
    if (R_FAILED(rc)) {
        printf("Failed to set reboot payload: 0x%x\n", rc);
    }
    else {
        spsmShutdown(true);
    }
}

GuiHekate::GuiHekate() : Gui() {
    canReboot = true;
    restrictedMode = false;
    errorMessage.clear();
    Result rc = 0;

    if (R_SUCCEEDED(rc = setsysInitialize())) {
        SetSysProductModel model;
        setsysGetProductModel(&model);
        setsysExit();
        if (model != SetSysProductModel_Nx && model != SetSysProductModel_Copper) {
            restrictedMode = true;

            if (model == SetSysProductModel_Iowa || model == SetSysProductModel_Hoag || model == SetSysProductModel_Calcio || model == SetSysProductModel_Aula) {
                errorMessage = "Unsupported switch model (Mariko)";
            }
            else {
                errorMessage = "Unknown switch model";
            }
            
        }
    }

    if (canReboot && R_FAILED(rc = spsmInitialize())) {
        // set restricted mode so the hekate stuff isn't loaded in
        restrictedMode = true;
        canReboot = false;
        errorMessage = "Failed to initialize spsm!";
    }

    if (canReboot && !restrictedMode) {
        smExit(); //Required to connect to ams:bpc
        if (R_FAILED(rc = amsBpcInitialize())) {
            restrictedMode = true;
            errorMessage = "Failed to initialize ams:bpc!";
            smInitialize();
        }
    }

    if (canReboot && !restrictedMode) {
        FILE *f = fopen("sdmc:/bootloader/update.bin", "rb");
        if (f == NULL) {
            restrictedMode = true;
            errorMessage = "Can't find \"bootloader/update.bin\"!";
        }
    }

    if (restrictedMode) {
        initializeForRestrictedMode();
    }
    else {
        InitializeRegular();
    }

    endInit();
}

void GuiHekate::initializeForRestrictedMode() {
    auto shutdownButton = new Button();
    shutdownButton->usableCondition = [this]() -> bool { return canReboot; };
    shutdownButton->volume = {Gui::g_framebuffer_width - 800, 80};
    shutdownButton->position = {100, Gui::g_framebuffer_height / 2 - shutdownButton->volume.second / 2};
    shutdownButton->adjacentButton[ADJ_RIGHT] = 1;
    shutdownButton->drawAction = [shutdownButton](Gui *gui, u16 x, u16 y, bool *isActivated) {
        gui->drawRectangled(x, y, shutdownButton->volume.first, shutdownButton->volume.second, currTheme.submenuButtonColor);
        gui->drawTextAligned(font20, x + shutdownButton->volume.first / 2, y + shutdownButton->volume.second / 2 + 10, currTheme.textColor, "Shutdown", ALIGNED_CENTER);
    };
    shutdownButton->inputAction = [&](u32 kdown, bool *isActivated) {
        if (kdown & HidNpadButton_A) {
            spsmShutdown(false);
        }
    };
    add(shutdownButton);

    auto rebootButton = new Button();
    rebootButton->usableCondition = [this]() -> bool { return canReboot; };
    rebootButton->volume = {Gui::g_framebuffer_width - 800, 80};
    rebootButton->position = {Gui::g_framebuffer_width - 100 - rebootButton->volume.first, Gui::g_framebuffer_height / 2 - rebootButton->volume.second / 2};
    rebootButton->adjacentButton[ADJ_LEFT] = 0;
    rebootButton->drawAction = [rebootButton](Gui *gui, u16 x, u16 y, bool *isActivated) {
        gui->drawRectangled(x, y, rebootButton->volume.first, rebootButton->volume.second, currTheme.submenuButtonColor);
        gui->drawTextAligned(font20, x + rebootButton->volume.first / 2, y + rebootButton->volume.second / 2 + 10, currTheme.textColor, "Reboot to payload", ALIGNED_CENTER);
    };
    rebootButton->inputAction = [&](u32 kdown, bool *isActivated) {
        if (kdown & HidNpadButton_A) {
            spsmShutdown(true);
        }
    };
    add(rebootButton);
}

void GuiHekate::InitializeRegular() {
    static std::vector<std::string> rebootNames;
    static u16 currRebootEntryIndex;

    getBootConfigs(m_rebootConfigs, currRebootEntryIndex);
    m_rebootConfigs.push_back({"Boot to UMS (SD Card)", 0, false, true});
    //m_currRebootConfig = m_rebootConfigs[0];

    auto profileButton = new Button();
    profileButton->usableCondition = [this]() -> bool { return canReboot; };
    profileButton->position = {200, 250};
    profileButton->adjacentButton[ADJ_DOWN] = 1;
    profileButton->volume = {Gui::g_framebuffer_width - 400, 80};
    profileButton->drawAction = [&](Gui *gui, u16 x, u16 y, bool *isActivated) {
        gui->drawTextAligned(font20, x + 37, y + 50, currTheme.textColor, "Hekate profile", ALIGNED_LEFT);
        std::string autoBootName = m_currRebootConfig.name;

        if (autoBootName.length() >= 25) {
            autoBootName = autoBootName.substr(0, 24);
            autoBootName += "...";
        }

        gui->drawTextAligned(font20, x + 830, y + 50, currTheme.selectedColor, autoBootName.c_str(), ALIGNED_RIGHT);
    };
    profileButton->inputAction = [&](u32 kdown, bool *isActivated) {
        if (kdown & HidNpadButton_A) {
            rebootNames.clear();

            for (auto const &autoBootEntry : m_rebootConfigs)
                rebootNames.push_back(autoBootEntry.name);

            (new ListSelector("Hekate profile to reboot to", "\uE0E1 Back     \uE0E0 OK", rebootNames, currRebootEntryIndex))
                ->setInputAction([&](u32 k, u16 selectedItem) {
                    if (k & HidNpadButton_A) {
                        currRebootEntryIndex = selectedItem;
                        m_currRebootConfig = m_rebootConfigs[selectedItem];

                        Gui::g_currListSelector->hide();
                    }
                })
                ->show();
        }
    };
    add(profileButton);

    auto rebootButton = new Button();
    rebootButton->usableCondition = [this]() -> bool { return canReboot; };
    rebootButton->position = {400, 450};
    rebootButton->volume = {Gui::g_framebuffer_width - 800, 80};
    rebootButton->adjacentButton[ADJ_UP] = 0;
    rebootButton->drawAction = [&](Gui *gui, u16 x, u16 y, bool *isActivated) {
        gui->drawRectangled(x, y, Gui::g_framebuffer_width - 800, 80, currTheme.submenuButtonColor);
        gui->drawTextAligned(font20, Gui::g_framebuffer_width / 2, y + 50, currTheme.textColor, "Reboot now!", ALIGNED_CENTER);
    };
    rebootButton->inputAction = [&](u32 kdown, bool *isActivated) {
        if (kdown & HidNpadButton_A) {
            FILE *f = fopen("sdmc:/bootloader/update.bin", "rb");
            if (f) {
                fread(g_reboot_payload, 1, sizeof(g_reboot_payload), f);
                fclose(f);

                if (m_currRebootConfig.ums) {
                    // CTCaer told me to add that (Force go to menu)
                    g_reboot_payload[0x94] = 1;
                    g_reboot_payload[0x95] = 0;
                    g_reboot_payload[0x96] = 0;
                    // sets the UMS bit
                    g_reboot_payload[0x97] = 1 << 5;
                    // selects the UMS type
                    g_reboot_payload[0x98] = NyxUMSType::NYX_UMS_SD_CARD;
                } else {
                    g_reboot_payload[0x94] = 1;
                    g_reboot_payload[0x95] = m_currRebootConfig.id;
                    g_reboot_payload[0x96] = m_currRebootConfig.autoBootList;
                }

                reboot_to_payload();
            } else
                (new MessageBox("Can't find \"bootloader/update.bin\"!", MessageBox::OKAY))->show();
        }
    };
    add(rebootButton);
}

GuiHekate::~GuiHekate() {
    if (serviceIsActive(amsBpcGetServiceSession())) {
        amsBpcExit();
    }

    if(!serviceIsActive(smGetServiceSession())) {
        smInitialize();
    }

    if (serviceIsActive(spsmGetServiceSession())) {
        spsmExit();
    }
}

void GuiHekate::draw() {
    Gui::beginDraw();

    Gui::drawRectangle(0, 0, Gui::g_framebuffer_width, Gui::g_framebuffer_height, currTheme.backgroundColor);
    Gui::drawRectangle((u32)((Gui::g_framebuffer_width - 1220) / 2), 87, 1220, 1, currTheme.textColor);
    Gui::drawRectangle((u32)((Gui::g_framebuffer_width - 1220) / 2), Gui::g_framebuffer_height - 73, 1220, 1, currTheme.textColor);
    Gui::drawTextAligned(fontIcons, 70, 68, currTheme.textColor, "\uE130", ALIGNED_LEFT);
    Gui::drawTextAligned(font24, 70, 58, currTheme.textColor, "        Hekate Toolbox", ALIGNED_LEFT);
    Gui::drawTextAligned(font20, Gui::g_framebuffer_width - 50, Gui::g_framebuffer_height - 25, currTheme.textColor, "\uE0E1 Back     \uE0E0 OK", ALIGNED_RIGHT);

    if (canReboot)
    {
        if (restrictedMode)
        {
            Gui::drawTextAligned(font24, Gui::g_framebuffer_width / 2, 150, currTheme.activatedColor, "Restricted mode", ALIGNED_CENTER);
            Gui::drawTextAligned(font20, Gui::g_framebuffer_width / 2, 200, currTheme.textColor, "Rebooting directly to hekate is not possible,\nbut you can choose one of these two options:", ALIGNED_CENTER);

            const std::string restrictedModeReason = "Restricted mode reason: " + errorMessage;
            Gui::drawTextAligned(font20, Gui::g_framebuffer_width / 2, Gui::g_framebuffer_height - 150, currTheme.textColor, restrictedModeReason.c_str(), ALIGNED_CENTER);
        }
        else
        {
            Gui::drawTextAligned(font20, Gui::g_framebuffer_width / 2, 150, currTheme.textColor, "Select the Hekate profile you want to reboot your Nintendo Switch into. \n Make sure to close all open titles beforehand as this will reboot your device immediately.", ALIGNED_CENTER);
        }
    }
    else
    {
        Gui::drawTextAligned(font20, Gui::g_framebuffer_width / 2, 150, currTheme.activatedColor, errorMessage.c_str(), ALIGNED_CENTER);
    }

    drawButtons();

    Gui::endDraw();
}

void GuiHekate::onInput(u32 kdown) {
    inputButtons(kdown);

    if (kdown & HidNpadButton_B)
        Gui::g_nextGui = GUI_MAIN;
}
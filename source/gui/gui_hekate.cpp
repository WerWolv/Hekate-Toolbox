#include "gui_hekate.hpp"
#include "button.hpp"

#include <stdio.h>
#include <dirent.h>
#include <sstream>
#include <fstream>
#include <utility>
#include "SimpleIniParser.hpp"

#include "list_selector.hpp"

#define IRAM_PAYLOAD_MAX_SIZE 0x2F000
#define IRAM_PAYLOAD_BASE     0x40010000

static __attribute__((aligned(0x1000))) u8 g_reboot_payload[IRAM_PAYLOAD_MAX_SIZE];
static __attribute__((aligned(0x1000))) u8 g_ff_page[0x1000];
static __attribute__((aligned(0x1000))) u8 g_work_page[0x1000];

void do_iram_dram_copy(void *buf, uintptr_t iram_addr, size_t size, int option) {
    memcpy(g_work_page, buf, size);

    SecmonArgs args = {0};
    args.X[0] = 0xF0000201;             /* smcAmsIramCopy */
    args.X[1] = (uintptr_t)g_work_page; /* DRAM Address */
    args.X[2] = iram_addr;              /* IRAM Address */
    args.X[3] = size;                   /* Copy size */
    args.X[4] = option;                 /* 0 = Read, 1 = Write */
    svcCallSecureMonitor(&args);

    memcpy(buf, g_work_page, size);
}

void copy_to_iram(uintptr_t iram_addr, void *buf, size_t size) {
    do_iram_dram_copy(buf, iram_addr, size, 1);
}

void copy_from_iram(void *buf, uintptr_t iram_addr, size_t size) {
    do_iram_dram_copy(buf, iram_addr, size, 0);
}

static void clear_iram(void) {
    memset(g_ff_page, 0xFF, sizeof(g_ff_page));
    for (size_t i = 0; i < IRAM_PAYLOAD_MAX_SIZE; i += sizeof(g_ff_page)) {
        copy_to_iram(IRAM_PAYLOAD_BASE + i, g_ff_page, sizeof(g_ff_page));
    }
}

static void reboot_to_payload(void) {
    clear_iram();

    for (size_t i = 0; i < IRAM_PAYLOAD_MAX_SIZE; i += 0x1000) {
        copy_to_iram(IRAM_PAYLOAD_BASE + i, &g_reboot_payload[i], 0x1000);
    }

    splSetConfig((SplConfigItem)65001, 2);
}

GuiHekate::GuiHekate() : Gui() {
    static std::vector<std::string> rebootNames;
    static u16 currRebootEntryIndex;

    splInitialize();

    m_rebootConfigs.push_back({"Hekate menu", 0, false});
    getBootConfigs(m_rebootConfigs, currRebootEntryIndex);
    //m_currRebootConfig = m_rebootConfigs[0];

    auto profileButton = Button();
    profileButton.position = {200, 250};
    profileButton.adjacentButton[ADJ_DOWN] = 1;
    profileButton.volume = {Gui::g_framebuffer_width - 400, 80};
    profileButton.drawAction = [&](Gui *gui, u16 x, u16 y, bool *isActivated) {
        gui->drawTextAligned(font20, x + 37, y + 50, currTheme.textColor, "Hekate profile", ALIGNED_LEFT);
        std::string autoBootName = m_currRebootConfig.name;

        if (autoBootName.length() >= 25) {
            autoBootName = autoBootName.substr(0, 24);
            autoBootName += "...";
        }

        gui->drawTextAligned(font20, x + 830, y + 50, currTheme.selectedColor, autoBootName.c_str(), ALIGNED_RIGHT);
    };
    profileButton.inputAction = [&](u32 kdown, bool *isActivated) {
        if (kdown & KEY_A) {
            rebootNames.clear();

            for (auto const &autoBootEntry : m_rebootConfigs)
                rebootNames.push_back(autoBootEntry.name);

            (new ListSelector("Hekate profile to reboot to", "\uE0E1 Back     \uE0E0 OK", rebootNames, currRebootEntryIndex))
                ->setInputAction([&](u32 k, u16 selectedItem) {
                    if (k & KEY_A) {
                        currRebootEntryIndex = selectedItem;
                        m_currRebootConfig = m_rebootConfigs[selectedItem];

                        Gui::g_currListSelector->hide();
                    }
                })
                ->show();
        }
    };
    add(profileButton);

    auto rebootButton = Button();
    rebootButton.position = {400, 450};
    rebootButton.volume = {Gui::g_framebuffer_width - 800, 80};
    rebootButton.adjacentButton[ADJ_UP] = 0;
    rebootButton.drawAction = [&](Gui *gui, u16 x, u16 y, bool *isActivated) {
        gui->drawRectangled(x, y, Gui::g_framebuffer_width - 800, 80, currTheme.submenuButtonColor);
        gui->drawTextAligned(font20, Gui::g_framebuffer_width / 2, y + 50, currTheme.textColor, "Reboot now!", ALIGNED_CENTER);
    };
    rebootButton.inputAction = [&](u32 kdown, bool *isActivated) {
        if (kdown & KEY_A) {
            FILE *f = fopen("sdmc:/bootloader/update.bin", "rb");
            if (f) {
                fread(g_reboot_payload, 1, sizeof(g_reboot_payload), f);
                fclose(f);

                g_reboot_payload[0x94] = 1;
                g_reboot_payload[0x95] = m_currRebootConfig.id;
                g_reboot_payload[0x96] = m_currRebootConfig.autoBootList;

                reboot_to_payload();
            } else
                (new MessageBox("Can't find \"bootloader/update.bin\"!", MessageBox::OKAY))->show();
        }
    };
    add(rebootButton);
    endInit();
}

GuiHekate::~GuiHekate() {
    splExit();
}

void GuiHekate::draw() {
    Gui::beginDraw();

    Gui::drawRectangle(0, 0, Gui::g_framebuffer_width, Gui::g_framebuffer_height, currTheme.backgroundColor);
    Gui::drawRectangle((u32)((Gui::g_framebuffer_width - 1220) / 2), 87, 1220, 1, currTheme.textColor);
    Gui::drawRectangle((u32)((Gui::g_framebuffer_width - 1220) / 2), Gui::g_framebuffer_height - 73, 1220, 1, currTheme.textColor);
    Gui::drawTextAligned(fontIcons, 70, 68, currTheme.textColor, "\uE130", ALIGNED_LEFT);
    Gui::drawTextAligned(font24, 70, 58, currTheme.textColor, "        Hekate Toolbox", ALIGNED_LEFT);
    Gui::drawTextAligned(font20, Gui::g_framebuffer_width - 50, Gui::g_framebuffer_height - 25, currTheme.textColor, "\uE0E1 Back     \uE0E0 OK", ALIGNED_RIGHT);

    Gui::drawTextAligned(font20, Gui::g_framebuffer_width / 2, 150, currTheme.textColor, "Select the Hekate profile you want to reboot your Nintendo Switch into. \n Make sure to close all open titles beforehand as this will reboot your device immediately.", ALIGNED_CENTER);

    drawButtons();

    Gui::endDraw();
}

void GuiHekate::onInput(u32 kdown) {
    inputButtons(kdown);

    if (kdown & KEY_B)
        Gui::g_nextGui = GUI_MAIN;
}
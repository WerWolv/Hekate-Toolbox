#include <string.h>
#include <stdio.h>
#include <chrono>

#include <switch.h>
#include <thread>

#include "gui.hpp"
#include "gui_main.hpp"
#include "gui_sysmodule.hpp"
#include "gui_hekate.hpp"
#include "gui_overrides_menu.hpp"
#include "gui_override_key.hpp"
#include "gui_title_list.hpp"
#include "button.hpp"
#include "titleinfo.hpp"

#include "threads.hpp"

#define KREPEAT_MIN_HOLD 10
#define KREPEAT_INTERVAL 4

static Gui *currGui = nullptr;
static bool updateThreadRunning = false;
static Mutex mutexCurrGui;
u32 __nx_applet_type = AppletType_Default;

bool g_exitApplet = false;

void update() {
    while (updateThreadRunning) {
        auto begin = std::chrono::steady_clock::now();

        mutexLock(&mutexCurrGui);
        if (currGui != nullptr)
            currGui->update();

        mutexUnlock(&mutexCurrGui);

        svcSleepThread(1.0E6 - std::chrono::duration<double, std::nano>(std::chrono::steady_clock::now() - begin).count());
    }
}

int main(int argc, char **argv) {
    PadState pad;
    PadRepeater padRepeater;
    s32 prev_touchcount=0;

    socketInitializeDefault();
    nxlinkStdio();

    setsysInitialize();
    ColorSetId colorSetId;
    setsysGetColorSetId(&colorSetId);
    setTheme(colorSetId);
    setsysExit();
    nsInitialize();

    framebufferCreate(&Gui::g_fb_obj, nwindowGetDefault(), 1280, 720, PIXEL_FORMAT_RGBA_8888, 2);
    framebufferMakeLinear(&Gui::g_fb_obj);

    initJpegThread();

    Gui::g_nextGui = GUI_MAIN;

    mutexInit(&mutexCurrGui);

    updateThreadRunning = true;
    std::thread updateThread(update);

    // Initialize our input (max players, accept input from all controllers, enable repeat input initialize touchscreen)
    padConfigureInput(8, HidNpadStyleSet_NpadStandard);
    padInitializeAny(&pad);
    padRepeaterInitialize(&padRepeater, KREPEAT_MIN_HOLD, KREPEAT_INTERVAL);
    hidInitializeTouchScreen();

    while (appletMainLoop()) {
        padUpdate(&pad);
        // accept repeat input but only from directional buttons
        padRepeaterUpdate(&padRepeater, padGetButtons(&pad) & (HidNpadButton_AnyLeft | HidNpadButton_AnyUp | HidNpadButton_AnyRight | HidNpadButton_AnyDown));
        u64 kdown = padGetButtonsDown(&pad) | padRepeaterGetButtons(&padRepeater);

        if (Gui::g_nextGui != GUI_INVALID) {
            mutexLock(&mutexCurrGui);

            delete currGui;

            switch (Gui::g_nextGui) {
                case GUI_MAIN:
                    currGui = new GuiMain();
                    break;
                case GUI_SM_SELECT:
                    currGui = new GuiSysmodule();
                    break;
                case GUI_HEKATE:
                    currGui = new GuiHekate();
                    break;
                case GUI_OVERRIDES_MENU:
                    currGui = new GuiOverridesMenu();
                    break;
                case GUI_OVERRIDE_KEY:
                    currGui = new GuiOverrideKey();
                    break;
                case GUI_TITLE_LIST:
                    currGui = new GuiTitleList();
                default:
                    break;
            }

            mutexUnlock(&mutexCurrGui);
            Gui::g_nextGui = GUI_INVALID;
        }

        if (currGui != nullptr) {
            currGui->draw();

            if (kdown) {
                if (Gui::g_currListSelector != nullptr)
                    Gui::g_currListSelector->onInput(kdown);
                else if (Gui::g_currMessageBox != nullptr)
                    Gui::g_currMessageBox->onInput(kdown);
                else if ((kdown & HidNpadButton_Plus) && !Gui::g_exitBlocked)
                    break;
                else
                    currGui->onInput(kdown);
            }

            HidTouchScreenState state={0};
            if (hidGetTouchScreenStates(&state, 1)) {
                if (state.count > prev_touchcount) {
                    if (Gui::g_currListSelector != nullptr)
                        Gui::g_currListSelector->onTouch(state.touches[0]);
                    else if (Gui::g_currMessageBox != nullptr)
                        Gui::g_currMessageBox->onTouch(state.touches[0]);
                    else
                        currGui->onTouch(state.touches[0]);
                }
                prev_touchcount = state.count;
            }

            if (g_exitApplet)
                break;
        }
    }

    if (currGui != nullptr)
        delete currGui;

    updateThreadRunning = false;
    updateThread.join();

    exitJpegThread();

    nsExit();
    socketExit();
    framebufferClose(&Gui::g_fb_obj);

    appletEndBlockingHomeButton();

    return 0;
}

#include "gui_sysmodule.hpp"
#include "button.hpp"

#include <stdio.h>
#include <dirent.h>
#include <sstream>
#include <fstream>
#include <utility>

#include "list_selector.hpp"
#include "message_box.hpp"

#include "json.hpp"

#define CONTENTS_PATH "/atmosphere/contents/"

//Rows of buttons per column
#define ROWS 3

using json = nlohmann::json;

extern "C" {
#include "pm_dmnt.h"
#include "hid_extra.h"
}

static bool anyModulesPresent = false;

GuiSysmodule::GuiSysmodule() : Gui() {
    pmshellInitialize();
    pmdmntInitialize();

    anyModulesPresent = false;

    std::ifstream configFile("sdmc:/switch/HekateToolbox/config.json");
    json configJson;

    if (!configFile.fail()) {
        try {
            configFile >> configJson;
        } catch (json::parse_error &e) {}
    }

    DIR *contents_dir = opendir(CONTENTS_PATH);
    if (contents_dir != nullptr) {
        json toolboxJson;
        struct dirent *ent;
        while ((ent = readdir(contents_dir)) != nullptr) {
            std::ifstream sysconfig(CONTENTS_PATH + std::string(ent->d_name) + "/toolbox.json");
            if (!sysconfig.fail()) {
                try {
                    sysconfig >> toolboxJson;
                    configJson["sysmodules"].push_back(toolboxJson);
                } catch (json::parse_error &e) {}
            }
        }
    }
    closedir(contents_dir);

    for (auto sysmodule : configJson["sysmodules"]) {
        try {
            std::stringstream path;
            path << CONTENTS_PATH << sysmodule["tid"].get<std::string>() << "/exefs.nsp";

            if (access(path.str().c_str(), F_OK) == -1) continue;

            this->m_sysmodules.insert(std::make_pair(sysmodule["tid"].get<std::string>(), (sysModule_t){sysmodule["name"].get<std::string>(), sysmodule["tid"].get<std::string>(), sysmodule["requires_reboot"].get<bool>()}));

            u64 sysmodulePid = 0;
            pmdmntGetProcessId(&sysmodulePid, std::stoul(sysmodule["tid"].get<std::string>(), nullptr, 16));

            if (sysmodulePid > 0)
                this->m_runningSysmodules.insert(sysmodule["tid"].get<std::string>());

        } catch (json::parse_error &e) {
            continue;
        }
    }

    u16 xOffset = 0, yOffset = 0;
    s32 cnt = 0;

    for (auto &sysmodule : this->m_sysmodules) {
        FILE *exefs = fopen(std::string(CONTENTS_PATH + sysmodule.second.titleID + "/exefs.nsp").c_str(), "r");

        if (exefs == nullptr)
            continue;

        fclose(exefs);

        anyModulesPresent = true;

        auto sysmoduleButton = new Button();
        sysmoduleButton->position = {100 + xOffset, 250 + yOffset};
        sysmoduleButton->volume = {500, 80};
        sysmoduleButton->drawAction = [&](Gui *gui, u16 x, u16 y, bool *isActivated) {
            gui->drawTextAligned(font20, x + 37, y + 50, currTheme.textColor, sysmodule.second.name.c_str(), ALIGNED_LEFT);
            gui->drawTextAligned(font20, x + 420, y + 50, this->m_runningSysmodules.find(sysmodule.first) != this->m_runningSysmodules.end() ? currTheme.selectedColor : Gui::makeColor(0xB8, 0xBB, 0xC2, 0xFF), this->m_runningSysmodules.find(sysmodule.first) != this->m_runningSysmodules.end() ? "On" : "Off", ALIGNED_LEFT);
        };
        sysmoduleButton->inputAction = [&](u32 kdown, bool *isActivated) {
            if (kdown & HidNpadButton_A) {
                u64 pid;
                u64 tid = std::stol(sysmodule.first.c_str(), nullptr, 16);

                mkdir(std::string(CONTENTS_PATH + sysmodule.second.titleID + "/flags").c_str(), 777);
                std::stringstream path;
                path << CONTENTS_PATH << sysmodule.first << "/flags/boot2.flag";

                if (this->m_runningSysmodules.find(sysmodule.first) != this->m_runningSysmodules.end()) {
                    if (!sysmodule.second.requiresReboot) {
                        pmshellTerminateProgram(tid);
                    } else {
                        (new MessageBox("This sysmodule requires a reboot to fully work. \n Please restart your console in order use it.", MessageBox::OKAY))->show();
                    }

                    remove(path.str().c_str());
                } else {
                    if (sysmodule.second.requiresReboot) {
                        (new MessageBox("This sysmodule requires a reboot to fully work. \n Please restart your console in order use it.", MessageBox::OKAY))->show();
                        FILE *fptr = fopen(path.str().c_str(), "wb+");
                        if (fptr != nullptr) fclose(fptr);
                    } else {
                        NcmProgramLocation programLocation{
                            .program_id = tid,
                            .storageID = NcmStorageId_None,
                        };
                        if (R_SUCCEEDED(pmshellLaunchProgram(0, &programLocation, &pid))) {
                            FILE *fptr = fopen(path.str().c_str(), "wb+");
                            if (fptr != nullptr) fclose(fptr);
                        }
                    }
                }

                pid = 0;
                pmdmntGetProcessId(&pid, tid);

                if (!sysmodule.second.requiresReboot) {
                    if (pid != 0)
                        this->m_runningSysmodules.insert(sysmodule.first);
                    else
                        this->m_runningSysmodules.erase(sysmodule.first);
                } else {
                    if (access(path.str().c_str(), F_OK) == 0)
                        this->m_runningSysmodules.insert(sysmodule.first);
                    else
                        this->m_runningSysmodules.erase(sysmodule.first);
                }
            }
        };
        sysmoduleButton->adjacentButton = {
            (cnt % ROWS) != 0 ? (cnt - 1) : -1,        //UP
            (cnt % ROWS) != ROWS - 1 ? (cnt + 1) : -1, //DOWN
            (cnt - ROWS),                              //LEFT
            (cnt + ROWS)                               //RIGHT
        };
        add(sysmoduleButton);

        yOffset += 100;

        if (yOffset == ROWS * 100) {
            xOffset += 550;
            yOffset = 0;
        }

        cnt++;
    }
    selectButton(selection);
    endInit();
}

GuiSysmodule::~GuiSysmodule() {
    selection = getSelectedButtonIndex();
    pmshellExit();
    pmdmntExit();
}

void GuiSysmodule::draw() {
    Gui::beginDraw();
    Gui::drawRectangle(0, 0, Gui::g_framebuffer_width, Gui::g_framebuffer_height, currTheme.backgroundColor);
    Gui::drawRectangle((u32)((Gui::g_framebuffer_width - 1220) / 2), 87, 1220, 1, currTheme.textColor);
    Gui::drawRectangle((u32)((Gui::g_framebuffer_width - 1220) / 2), Gui::g_framebuffer_height - 73, 1220, 1, currTheme.textColor);
    Gui::drawTextAligned(fontIcons, 70, 68, currTheme.textColor, "\uE130", ALIGNED_LEFT);
    Gui::drawTextAligned(font24, 70, 58, currTheme.textColor, "        Hekate Toolbox", ALIGNED_LEFT);

    if (hidMitmInstalled())
        Gui::drawTextAligned(font20, Gui::g_framebuffer_width - 50, Gui::g_framebuffer_height - 25, currTheme.textColor, "\uE0E2 Key configuration     \uE0E1 Back     \uE0E0 OK", ALIGNED_RIGHT);
    else
        Gui::drawTextAligned(font20, Gui::g_framebuffer_width - 50, Gui::g_framebuffer_height - 25, currTheme.textColor, "\uE0E1 Back     \uE0E0 OK", ALIGNED_RIGHT);

    if (anyModulesPresent)
        Gui::drawTextAligned(font20, Gui::g_framebuffer_width / 2, 150, currTheme.textColor, "Select the background services (sysmodules) that should be running. \n Because of memory restraints it may be not possible to start all services at once.", ALIGNED_CENTER);
    else
        Gui::drawTextAligned(font20, Gui::g_framebuffer_width / 2, 550, currTheme.textColor, "You currently don't have any supported sysmodules installed. To use this \n feature, please install any supported sysmodule as an NSP.", ALIGNED_CENTER);

    drawButtons();
    Gui::endDraw();
}

void GuiSysmodule::onInput(u32 kdown) {
    inputButtons(kdown);

    if (kdown & HidNpadButton_B)
        Gui::g_nextGui = GUI_MAIN;

    if (hidMitmInstalled() && kdown & HidNpadButton_X)
        Gui::g_nextGui = GUI_HID_MITM;
}
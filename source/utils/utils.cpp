#include "utils.hpp"
#include "SimpleIniParser.hpp"
#include <stdio.h>
#include <dirent.h>
#include <algorithm>
#include <string.h>
#include "theme.h"

void getBootConfigs(std::vector<BootEntry> &out_bootEntries, u16 &currAutoBootEntryIndex) {
    out_bootEntries.clear();
    out_bootEntries.push_back({"Hekate menu", 0, false});
    currAutoBootEntryIndex = 0;

    u16 id;
    u16 currAutoboot = 0;           // autoboot entry index
    bool currAutoboot_list = false; // if autoboot index is for hekate_ipl.ini or ini folder

    //Process main hekate_ipl.ini file for boot entries
    auto fileIni = simpleIniParser::Ini::parseFile(HEKATE_INI);
    if (fileIni) {

        // get autoboot options from config file
        auto configSection = fileIni->findSection("config");
        if (configSection) {
            currAutoboot = std::stoi(configSection->findOrCreateFirstOption("autoboot", "0")->value);
            currAutoboot_list = std::stoi(configSection->findOrCreateFirstOption("autoboot_list", "0")->value);
        }

        // get boot entries from config file
        id = 0;
        for (auto const &it : fileIni->sections) {
            if (it->type != simpleIniParser::IniSectionType::Section || !strcmp("config", it->value.c_str())) continue;
            out_bootEntries.push_back({it->value, ++id, false});

            if (currAutoboot_list == false && id == currAutoboot) {
                currAutoBootEntryIndex = out_bootEntries.size() - 1;
            }
        }
        delete fileIni;
    }

    //Process any file-based boot entries from ini folder
    DIR *dr = opendir(INI_PATH);
    struct dirent *de;
    std::vector<std::string> iniFiles;

    while ((de = readdir(dr)) != nullptr)
        iniFiles.push_back(de->d_name);
    closedir(dr);

    std::sort(iniFiles.begin(), iniFiles.end());

    if (!iniFiles.empty()) {

        // get boot entries from ini folder files
        id = 0;
        for (auto const &iniFile : iniFiles) {
            std::string file = std::string(INI_PATH) + iniFile;
            fileIni = simpleIniParser::Ini::parseFile(file);

            if (fileIni) {
                for (auto const &it : fileIni->sections) {
                    if (it->type != simpleIniParser::IniSectionType::Section) continue;
                    out_bootEntries.push_back({it->value, ++id, true});

                    if (currAutoboot_list == true && id == currAutoboot) {
                        currAutoBootEntryIndex = out_bootEntries.size() - 1;
                    }
                }
            }
            delete fileIni;
        }
    }
}

const char *GetAppletName(u64 appID) {
    switch (appID) {
        case AppletID::AppletController:
            return "Controller";
        case AppletID::AppletMyPage:
            return "User Profile";
        case AppletID::AppletShop:
            return "Nintendo eShop";
        case AppletID::AppletPhotoViewer:
            return "Album";
        default:
            return nullptr;
    }
}

const char *GetAppletIcon(u64 appID) {
    switch (appID) {
        case AppletID::AppletController:
            return "\uE136";
        case AppletID::AppletMyPage:
            return "\uE131";
        case AppletID::AppletShop:
            return "\uE133";
        case AppletID::AppletPhotoViewer:
            return "\uE134";
        default:
            return "\uE06B";
    }
}

color_t GetAppletColor(u64 appID) {
    switch (appID) {
        case AppletID::AppletController:
            return color_t{static_cast<u32>(RGBA8(0x77, 0x77, 0x77, 0xFF))};
        case AppletID::AppletShop:
            return color_t{static_cast<u32>(RGBA8(0xFF, 0xA0, 0x00, 0xFF))};
        case AppletID::AppletPhotoViewer:
            return color_t{static_cast<u32>(RGBA8(0x13, 0x73, 0xFD, 0xFF))};
        default:
            return currTheme.textColor;
    }
}

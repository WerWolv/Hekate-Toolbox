#include "utils.hpp"
#include "SimpleIniParser.hpp"
#include <stdio.h>
#include <dirent.h>
#include <algorithm>
#include "theme.h"

BootEntry getBootConfigs(std::vector<BootEntry> &out_bootEntries, u16 &currAutoBootEntryIndex) {
    simpleIniParser::Ini *hekateIni = simpleIniParser::Ini::parseFile(HEKATE_INI);

    u16 id = 0;
    BootEntry currEntry;

    currAutoBootEntryIndex = 0;

    u16 currAutoboot = std::stoi(hekateIni->findSection("config")->findFirstOption("autoboot")->value);
    bool currAutoboot_list = std::stoi(hekateIni->findSection("config")->findFirstOption("autoboot_list")->value);

    currEntry = out_bootEntries.back();

    for (auto const &it : hekateIni->sections) {
        if (std::string(it->value) == "config" || it->type != simpleIniParser::IniSectionType::Section) continue;
        out_bootEntries.push_back({it->value, ++id, false});

        if (!currAutoboot_list && id == currAutoboot) {
            currEntry = out_bootEntries.back();
            currAutoBootEntryIndex = out_bootEntries.size() - 1;
        }
    }

    DIR *dr = opendir(INI_PATH);
    struct dirent *de;
    std::vector<std::string> iniFiles;

    while ((de = readdir(dr)) != nullptr)
        iniFiles.push_back(de->d_name);
    closedir(dr);

    std::sort(iniFiles.begin(), iniFiles.end());

    id = 0;

    delete hekateIni;
    hekateIni = nullptr;

    if (iniFiles.empty()) return currEntry;

    for (auto const &iniFile : iniFiles) {
        std::string file = std::string(INI_PATH) + iniFile;
        hekateIni = simpleIniParser::Ini::parseFile(file);

        for (auto const &it : hekateIni->sections) {
            if (it->type != simpleIniParser::IniSectionType::Section) continue;
            out_bootEntries.push_back({it->value, ++id, true});

            if (currAutoboot_list && id == currAutoboot) {
                currEntry = out_bootEntries.back();
                currAutoBootEntryIndex = out_bootEntries.size() - 1;
            }
        }

        delete hekateIni;
    }

    return currEntry;
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

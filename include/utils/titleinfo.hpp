#pragma once
#include "switch.h"
#include <memory>
#include <vector>
#include "jpeg.hpp"

struct TitleIcon {
    u64 application_id;
    char name[0x200];
    std::unique_ptr<const u8> icon;
};

void initJpegThread();

void exitJpegThread();

//Dump a title's info based on the application ID. if width or height are specified, the icon will be set to that size.
std::shared_ptr<TitleIcon> DumpTitle(u64 appID, WidthHeight res = {});

//Dump all titles' application IDs.
std::vector<u64> DumpAllAppIDs();

//Dump all titles' info. if width or height are specified, the icons will be set to that size.
std::vector<std::shared_ptr<TitleIcon>> DumpAllTitles(WidthHeight res = {});

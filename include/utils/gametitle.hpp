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

//Dump a game's info based on the application ID. if width or height are specified, the icon will be set to that size.
std::shared_ptr<TitleIcon> DumpGame(u64 appID, WidthHeight res = {});

//Dump all games' application IDs.
std::vector<u64> DumpAllAppIDs();

//Dump all games' info. if width or height are specified, the icons will be set to that size.
std::vector<std::shared_ptr<TitleIcon>> DumpAllGames(WidthHeight res = {});

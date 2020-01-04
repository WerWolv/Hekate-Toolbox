#pragma once
#include "switch.h"
#include <memory>
#include <vector>

struct TitleIcon {
  u64 application_id;
  char name[0x200];
  std::unique_ptr<const u8> icon;
};

std::shared_ptr<TitleIcon> DumpGame(u64 appID);

std::vector<u64> DumpAllTitleIDs();

std::vector<std::shared_ptr<TitleIcon>> DumpAllGames();

#pragma once

#include <switch.h>
#include <string>
#include <vector>

#define LOADER_INI "sdmc:/atmosphere/loader.ini"
#define HEKATE_INI "sdmc:/bootloader/hekate_ipl.ini"
#define INI_PATH   "sdmc:/bootloader/ini/"

typedef struct {
  std::string name;
  u16 id;
  bool autoBootList;
} BootEntry;

BootEntry getBootConfigs(std::vector<BootEntry> &out_bootEntries, u16 &currAutoBootEntryIndex);
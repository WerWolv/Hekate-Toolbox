#include "utils.hpp"
#include "SimpleIniParser.hpp"
#include <stdio.h>
#include <dirent.h>
#include <algorithm>

BootEntry getBootConfigs(std::vector<BootEntry> &out_bootEntries, u16 &currAutoBootEntryIndex) {
  simpleIniParser::Ini *hekateIni = simpleIniParser::Ini::parseFile(HEKATE_INI);

  u16 id = 0;
  BootEntry currEntry;

  currAutoBootEntryIndex = 0;

  u16 currAutoboot = std::stoi(hekateIni->findSection("config")->findFirstOption("autoboot")->value);
  bool currAutoboot_list = std::stoi(hekateIni->findSection("config")->findFirstOption("autoboot_list")->value);

  currEntry = out_bootEntries.back();

  for (auto const& it : hekateIni->sections) {
    if(std::string(it->value) == "config" || it->type != simpleIniParser::IniSectionType::Section) continue;
    out_bootEntries.push_back({ it->value, ++id, false });

    if(!currAutoboot_list && id == currAutoboot) {
      currEntry = out_bootEntries.back();
      currAutoBootEntryIndex = out_bootEntries.size() - 1;
    }
  }

  DIR *dr = opendir(INI_PATH);
  struct dirent *de;
  std::vector<std::string> iniFiles;

  while((de = readdir(dr)) != nullptr)
    iniFiles.push_back(de->d_name);
  closedir(dr);

  std::sort(iniFiles.begin(), iniFiles.end());

  id = 0;

  delete hekateIni;
  hekateIni = nullptr;

  if (iniFiles.empty()) return currEntry;

  for(auto const& iniFile : iniFiles) {
    std::string file = std::string(INI_PATH) + iniFile;
    hekateIni = simpleIniParser::Ini::parseFile(file);

    for (auto const& it : hekateIni->sections) {
      if (it->type != simpleIniParser::IniSectionType::Section) continue;
      out_bootEntries.push_back({ it->value, ++id, true });

      if(currAutoboot_list && id == currAutoboot) {
        currEntry = out_bootEntries.back();
        currAutoBootEntryIndex = out_bootEntries.size() - 1;
      }
    }
	
	delete hekateIni;
  }
  
  return currEntry;
}
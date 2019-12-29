#pragma once

#include <switch.h>
#include <string>
#include <vector>

#define LOADER_INI    "sdmc:/atmosphere/config/override_config.ini"
#define HEKATE_INI    "sdmc:/bootloader/hekate_ipl.ini"
#define INI_PATH      "sdmc:/bootloader/ini/"
#define HID_MITM_INI  "sdmc:/config/hid_mitm/config.ini"

typedef struct {
  std::string name;
  u16 id;
  bool autoBootList;
} BootEntry;

BootEntry getBootConfigs(std::vector<BootEntry> &out_bootEntries, u16 &currAutoBootEntryIndex);

namespace ProgramID {
  constexpr u64 Invalid = 0x0;
  constexpr u64 AppletQlaunch         = 0x0100000000001000;
  constexpr u64 AppletAuth            = 0x0100000000001001;
  constexpr u64 AppletCabinet         = 0x0100000000001002;
  constexpr u64 AppletController      = 0x0100000000001003;
  constexpr u64 AppletDataErase       = 0x0100000000001004;
  constexpr u64 AppletError           = 0x0100000000001005;
  constexpr u64 AppletNetConnect      = 0x0100000000001006;
  constexpr u64 AppletPlayerSelect    = 0x0100000000001007;
  constexpr u64 AppletSwkbd           = 0x0100000000001008;
  constexpr u64 AppletMiiEdit         = 0x0100000000001009;
  constexpr u64 AppletWeb             = 0x010000000000100A;
  constexpr u64 AppletShop            = 0x010000000000100B;
  constexpr u64 AppletOverlayDisp     = 0x010000000000100C;
  constexpr u64 AppletPhotoViewer     = 0x010000000000100D;
  constexpr u64 AppletSet             = 0x010000000000100E;
  constexpr u64 AppletOfflineWeb      = 0x010000000000100F;
  constexpr u64 AppletLoginShare      = 0x0100000000001010;
  constexpr u64 AppletWifiWebAuth     = 0x0100000000001011;
  constexpr u64 AppletStarter         = 0x0100000000001012;
  constexpr u64 AppletMyPage          = 0x0100000000001013;
  constexpr u64 AppletPlayReport      = 0x0100000000001014;
  constexpr u64 AppletMaintenanceMenu = 0x0100000000001015;
  constexpr u64 AppletGift            = 0x010000000000101A;
  constexpr u64 AppletDummyShop       = 0x010000000000101B;
  constexpr u64 AppletUserMigration   = 0x010000000000101C;
  constexpr u64 AppletEncounter       = 0x010000000000101D;
  constexpr u64 AppletStory           = 0x0100000000001020;
}

#pragma once

#include <switch.h>

// hid-mitm communication

// Initializes the special hid-ipc by hid-mitm
Result hidExtraInitialize();

void hidExtraExit();

// Reloads hid-mitm rebind config
Result hidExtraReloadConfig();

// Pauses hid-mitm until the next config-reload
Result hidExtraPause();

bool hidMitmInstalled();

Result hidSysAcquireHomeButtonEventHandle(Event *event, bool autoclear);
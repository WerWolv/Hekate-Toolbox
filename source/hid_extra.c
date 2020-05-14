#include <switch.h>
#include "hid_extra.h"

static Service hid_service;

static bool hidExtraPaused = false;

Result hidExtraReloadConfig() {
    Result rc = serviceDispatch(&hid_service, 65000);
    hidExtraPaused = false;
    svcSleepThread(1e9L / 50); // Need to wait for hid to be polled again

    return rc;
}

Result hidExtraPause() {
    // we don't want to spam hid-mitm unnecessarily
    if (hidExtraPaused)
        return 0;

    Result rc = serviceDispatch(&hid_service, 65001);
    svcSleepThread(1e9L / 50); // Need to wait for hid to be polled again

    hidExtraPaused = true;

    return rc;
}

static bool hidExtraActive = false;
bool hidMitmInstalled() {
    return hidExtraActive;
}

Result hidExtraInitialize() {
    Result rc = smGetService(&hid_service, "hid");
    if (R_SUCCEEDED(rc))
        rc = hidExtraReloadConfig();
    if (R_SUCCEEDED(rc))
        hidExtraActive = true;
    if (R_FAILED(rc))
        serviceClose(&hid_service);

    return rc;
}

void hidExtraExit() {
    if (hidExtraActive)
        serviceClose(&hid_service);
    hidExtraActive = false;
}
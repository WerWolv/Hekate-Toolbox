#include <switch.h>
#include "hid_extra.h"

static Service hid_service;


static bool hidExtraPaused = false;

Result hidExtraReloadConfig()
{
    IpcCommand c;
    ipcInitialize(&c);

    struct
    {
        u64 magic;
        u64 cmd_id;
    } * raw;

    raw = ipcPrepareHeader(&c, sizeof(*raw));

    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 65000;

    Result rc = serviceIpcDispatch(&hid_service);

    if (R_SUCCEEDED(rc))
    {
        IpcParsedCommand r;
        ipcParse(&r);

        struct
        {
            u64 magic;
            u64 result;
        } *resp = r.Raw;

        rc = resp->result;
    }

    hidExtraPaused = false;
    svcSleepThread(1e9L/50); // Need to wait for hid to be polled again

    return rc;
}


Result hidExtraPause()
{
    // we don't want to spam hid-mitm unnecessarily
    if(hidExtraPaused)
        return 0;

    IpcCommand c;
    ipcInitialize(&c);

    struct
    {
        u64 magic;
        u64 cmd_id;
    } * raw;

    raw = ipcPrepareHeader(&c, sizeof(*raw));

    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 65001;

    Result rc = serviceIpcDispatch(&hid_service);

    if (R_SUCCEEDED(rc))
    {
        IpcParsedCommand r;
        ipcParse(&r);

        struct
        {
            u64 magic;
            u64 result;
        } *resp = r.Raw;

        rc = resp->result;
    }

    svcSleepThread(1e9L/50); // Need to wait for hid to be polled again

    hidExtraPaused = true;

    return rc;
}

static bool hidExtraActive = false;
bool hidMitmInstalled()
{
    return hidExtraActive;
}

Result hidExtraInitialize()
{
    Result rc = smGetService(&hid_service, "hid");
    if (R_SUCCEEDED(rc))
        rc = hidExtraReloadConfig();
    if (R_SUCCEEDED(rc))
        hidExtraActive = true;
    if (R_FAILED(rc))
        serviceClose(&hid_service);

    return rc;
}

void hidExtraExit()
{
    if (hidExtraActive)
        serviceClose(&hid_service);
    hidExtraActive = false;
}
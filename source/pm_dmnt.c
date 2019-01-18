#include "pm_dmnt.h"

static Service g_pmdmntSrv;

Result pmdmntInitialize_mod() {
    return smGetService(&g_pmdmntSrv, "pm:dmnt");
}

void pmdmntExit_mod() {
    serviceClose(&g_pmdmntSrv);
}

Result pmdmntGetCurrentLimitInfo(u64 *cur_val, u64 *lim_val, u32 category, u32 resource) {
    IpcCommand c;
    ipcInitialize(&c);

    struct {
        u64 magic;
        u64 cmd_id;
        u32 category;
        u32 resource;
    } *raw;

    raw = ipcPrepareHeader(&c, sizeof(*raw));

    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 65001;
    raw->category = category;
    raw->resource = resource;

    Result rc = serviceIpcDispatch(&g_pmdmntSrv);

    if (R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        ipcParse(&r);

        struct {
            u64 magic;
            u64 result;
            u64 cur_val;
            u64 lim_val;
        } *resp = r.Raw;

        rc = resp->result;

        if (R_SUCCEEDED(rc)) {
            *cur_val = resp->cur_val;
            *lim_val = resp->lim_val;
        }
    }

    return rc;
}
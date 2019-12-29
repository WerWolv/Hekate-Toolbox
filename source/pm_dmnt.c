#include "pm_dmnt.h"

static Service g_pmdmntSrv;

Result pmdmntInitialize_mod() {
    return smGetService(&g_pmdmntSrv, "pm:dmnt");
}

void pmdmntExit_mod() {
    serviceClose(&g_pmdmntSrv);
}

Result pmdmntGetCurrentLimitInfo(u64 *cur_val, u64 *lim_val, u32 category, u32 resource) {

    struct {
        u32 category;
        u32 resource;
    } in = {category, resource};

    struct {
        u64 cur_val;
        u64 lim_val;
    } out;

    Result rc = serviceDispatchInOut(&g_pmdmntSrv, 65001, in, out);
    if (R_SUCCEEDED(rc)) {
        *cur_val = out.cur_val;
        *lim_val = out.lim_val;
    }

    return rc;
}
#include "ldr_cfg.h"

static Service g_ldrCfgService;

Result ldrCfgInitialize() {
  return smGetService(&g_ldrCfgService, "ldr:cfg");
}

void ldrCfgExit() {
  serviceClose(&g_ldrCfgService);
}

Result getCurrOverrideKeyCombo(u64 *comboOut) {
    IpcCommand c;
    ipcInitialize(&c);

    struct {
        u64 magic;
        u64 cmd_id;
    } *raw;

    raw = ipcPrepareHeader(&c, sizeof(*raw));

    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 0;

    Result rc = serviceIpcDispatch(&g_ldrCfgService);

    if (R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        ipcParse(&r);

        struct {
            u64 magic;
            u64 result;
			u64 combo;
        } *resp = r.Raw;

        rc = resp->result;
		*comboOut = resp->combo;
    }

    return rc;
}

Result setCurrOverrideKeyCombo(u64 combo, u64 *comboOut) {
    IpcCommand c;
    ipcInitialize(&c);

    struct {
        u64 magic;
        u64 cmd_id;
		u64 combo;
    } *raw;

    raw = ipcPrepareHeader(&c, sizeof(*raw));

    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 1;
	raw->combo = combo;

    Result rc = serviceIpcDispatch(&g_ldrCfgService);

    if (R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        ipcParse(&r);

        struct {
            u64 magic;
            u64 result;
			u64 combo;
        } *resp = r.Raw;

        rc = resp->result;
		*comboOut = resp->combo;
    }

    return rc;
}

Result getOverrideByDefault(bool *enabledOut) {
    IpcCommand c;
    ipcInitialize(&c);

    struct {
        u64 magic;
        u64 cmd_id;
    } *raw;

    raw = ipcPrepareHeader(&c, sizeof(*raw));

    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 2;

    Result rc = serviceIpcDispatch(&g_ldrCfgService);

    if (R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        ipcParse(&r);

        struct {
            u64 magic;
            u64 result;
			bool enabled;
        } *resp = r.Raw;

        rc = resp->result;
		*enabledOut = resp->enabled;
    }

    return rc;
}

Result setOverrideByDefault(bool enabled, bool *enabledOut) {
    IpcCommand c;
    ipcInitialize(&c);

    struct {
        u64 magic;
        u64 cmd_id;
		u64 enabled;
    } *raw;

    raw = ipcPrepareHeader(&c, sizeof(*raw));

    raw->magic = SFCI_MAGIC;
    raw->cmd_id = 3;
	raw->enabled = enabled;

    Result rc = serviceIpcDispatch(&g_ldrCfgService);

    if (R_SUCCEEDED(rc)) {
        IpcParsedCommand r;
        ipcParse(&r);

        struct {
            u64 magic;
            u64 result;
			u64 enabled;
        } *resp = r.Raw;

        rc = resp->result;
		*enabledOut = resp->enabled;
    }

    return rc;
}

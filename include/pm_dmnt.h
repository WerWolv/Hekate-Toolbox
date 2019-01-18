#include <switch.h>

Result pmdmntInitialize_mod();
void pmdmntExit_mod();

Result pmdmntGetCurrentLimitInfo(u64 *cur_val, u64 *lim_val, u32 category, u32 resource);
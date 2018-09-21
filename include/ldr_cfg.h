#pragma once

#include <switch.h>

Result ldrCfgInitialize();
void ldrCfgExit();

Result getCurrOverrideKeyCombo(u64 *comboOut);
Result setCurrOverrideKeyCombo(u64 combo, u64 *comboOut);
Result getOverrideByDefault(bool *enabledOut);
Result setOverrideByDefault(bool enabled, bool *enabledOut);

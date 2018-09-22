#include <switch.h>
#include <stratosphere.hpp>
#include "ldr_cfg_service.hpp"

enum CfgServiceCmd {
	Cfg_Cmd_Get_KeyCombo = 0,
	Cfg_Cmd_Set_KeyCombo = 1,
	Cfg_Cmd_Get_Override = 2,
	Cfg_Cmd_Set_Override = 3,
};

Result LoaderConfigService::dispatch(IpcParsedCommand &r, IpcCommand &out_c, u64 cmd_id, u8 *pointer_buffer, size_t pointer_buffer_size) {
    Result rc = 0xF601;

    switch ((CfgServiceCmd)cmd_id) {
	    case Cfg_Cmd_Get_KeyCombo:
				rc = WrapIpcCommandImpl<&LoaderConfigService::getCurrOverrideKeyCombo>(this, r, out_c, pointer_buffer, pointer_buffer_size);
				break;
			case Cfg_Cmd_Set_KeyCombo:
				rc = WrapIpcCommandImpl<&LoaderConfigService::setCurrOverrideKeyCombo>(this, r, out_c, pointer_buffer, pointer_buffer_size);
				break;
			case Cfg_Cmd_Get_Override:
				rc = WrapIpcCommandImpl<&LoaderConfigService::getOverrideByDefault>(this, r, out_c, pointer_buffer, pointer_buffer_size);
				break;
			case Cfg_Cmd_Set_Override:
				rc = WrapIpcCommandImpl<&LoaderConfigService::setOverrideByDefault>(this, r, out_c, pointer_buffer, pointer_buffer_size);
				break;
      default:
          break;
    }
    return rc;
}

std::tuple<Result, u64> LoaderConfigService::getCurrOverrideKeyCombo() {
	if (g_override_key_combination == 0x00) return { 0x01, 0x00 };
	else return { 0x00, g_override_key_combination };
}

std::tuple<Result, u64> LoaderConfigService::setCurrOverrideKeyCombo(u64 combo) {
	g_override_key_combination = combo;

	return { 0x00, g_override_key_combination };
}

std::tuple<Result, bool> LoaderConfigService::getOverrideByDefault() {
	return { 0x00, g_override_by_default };
}

std::tuple<Result, bool> LoaderConfigService::setOverrideByDefault(bool enabled) {
	g_override_by_default = enabled;

	return { 0x00, g_override_by_default };
}

#pragma once
#include <switch.h>
#include <stratosphere/iserviceobject.hpp>

extern u64 g_override_key_combination;
extern bool g_override_by_default;

class LoaderConfigService final : public IServiceObject {
    public:
        Result dispatch(IpcParsedCommand &r, IpcCommand &out_c, u64 cmd_id, u8 *pointer_buffer, size_t pointer_buffer_size) override;
        Result handle_deferred() override {
            /* This service will never defer. */
            return 0;
        }

        LoaderConfigService *clone() override {
            return new LoaderConfigService();
        }

	private:
		std::tuple<Result, u64> getCurrOverrideKeyCombo();
		std::tuple<Result, u64> setCurrOverrideKeyCombo(u64 combo);
		std::tuple<Result, bool> getOverrideByDefault();
		std::tuple<Result, bool> setOverrideByDefault(bool enabled);
};

#pragma once

#include "gui.hpp"

#include <vector>
#include <string>
#include <unordered_map>
#include "utils.hpp"
#include "override_key.hpp"

class GuiMain : public Gui {
public:
    GuiMain();
    ~GuiMain() override;

    void draw() override;
    void onInput(u32 kdown) override;

private:
    static inline int selection = 0;
    OverrideKey m_albumOverrideKey;

    BootEntry m_currAutoBootConfig;
    std::vector<BootEntry> m_autoBootConfigs;
    std::vector<std::string> m_titleNames;
    std::vector<u64> m_titleIDs;
};

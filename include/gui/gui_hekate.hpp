#pragma once

#include "gui.hpp"

#include <vector>
#include <string>
#include <unordered_map>
#include <set>
#include "utils.hpp"

class GuiHekate : public Gui {
public:
    GuiHekate();
    ~GuiHekate() override;

    void draw() override;
    void onInput(u32 kdown) override;
    static inline BootEntry m_currRebootConfig{};

private:
    std::vector<BootEntry> m_rebootConfigs;
    bool canReboot;
    std::string errorMessage;
};

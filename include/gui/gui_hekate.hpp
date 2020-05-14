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
    ~GuiHekate();

    void update();
    void draw();
    void onInput(u32 kdown);
    void onTouch(touchPosition &touch);
    void onGesture(touchPosition &startPosition, touchPosition &endPosition);

    static inline BootEntry m_currRebootConfig{};

private:
    std::vector<BootEntry> m_rebootConfigs;
};

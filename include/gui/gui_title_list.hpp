#pragma once

#include "gui.hpp"

class GuiTitleList : public Gui {
public:
    GuiTitleList();
    ~GuiTitleList() override;

    void draw() override;
    void onInput(u32 kdown) override;
    void loadConfigFile();

    static inline u64 selectedAppID = 0;
};

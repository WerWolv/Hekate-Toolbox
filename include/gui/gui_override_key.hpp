#pragma once

#include "gui.hpp"
#include "override_key.hpp"

class GuiOverrideKey : public Gui {
public:
    GuiOverrideKey();
    ~GuiOverrideKey() override;

    void draw() override;
    void onInput(u32 kdown) override;
    void loadConfigFile();

private:
    ProgramOverrideKey m_override{};
    bool m_inputBlocked = false;
    bool m_overrideAnyApp = false;

public:
    static inline int selection = 2;
    static inline ProgramOverrideKey g_overrideKey{};
    static inline OverrideKeyType g_keyType{};
};

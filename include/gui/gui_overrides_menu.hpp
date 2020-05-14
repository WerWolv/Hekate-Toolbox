#pragma once

#include "gui.hpp"
#include "override_key.hpp"

enum class OverrideButtonType {
    Any_Title,
    Custom_Title,
    Add_New,
};

class GuiOverridesMenu : public Gui {
public:
    GuiOverridesMenu();
    ~GuiOverridesMenu();

    void update();
    void draw();
    void onInput(u32 kdown);
    void onTouch(touchPosition &touch);
    void onGesture(touchPosition &startPosition, touchPosition &endPosition);

    void addButton(OverrideButtonType buttonType, OverrideKeyType keyType = OverrideKeyType::Invalid, const ProgramOverrideKey &key = ProgramOverrideKey{});
    void loadConfigFile();
    void removeFromList(OverrideKeyType keyEntry);

private:
    ProgramOverrideKey m_overrides[8]{};
    ProgramOverrideKey m_anyAppOverride{};
    bool m_overrideAnyApp = false;
    u8 m_buttonCount = 0;
    bool displayDefaultOption = true;
    bool displayAnyTitleOption = true;

    OverrideKeyType m_selectedConfig;
    std::vector<OverrideKeyType> m_addConfigs;
    std::vector<std::tuple<OverrideButtonType, OverrideKeyType>> m_buttons;

public:
    static inline int selection = 0;
};

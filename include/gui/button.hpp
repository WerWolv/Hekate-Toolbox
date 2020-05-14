#pragma once

#include <switch.h>

#include <functional>
#include <vector>

class Gui;

enum AdjacentButtonDirection {
    ADJ_UP,
    ADJ_DOWN,
    ADJ_LEFT,
    ADJ_RIGHT,
};

class Button {
public:
    Button();

    void draw();
    bool onInput(u32 kdown);
    void onTouch(touchPosition &touch);

    inline bool isActivated() {
        return m_isActivated;
    }

    inline bool isSelected() {
        return m_isSelected;
    }

private:
    static void _defaultDrawAction(Gui *, u16, u16, bool *) {}
    static void _defaultInputAction(u32, bool *) {}
    static bool _defaultUsableCondition() { return true; }

public:
    std::pair<u16, u16> position{};
    std::pair<u16, u16> volume{};

    std::function<void(Gui *, u16, u16, bool *)> drawAction = _defaultDrawAction;
    std::function<void(u32, bool *)> inputAction = _defaultInputAction;
    std::function<bool()> usableCondition = _defaultUsableCondition;

    std::array<s32, 4> adjacentButton = {-1, -1, -1, -1};

    bool activatable = false;

private:
    bool m_isActivated;
    bool m_isSelected;
    Gui *m_gui;
    friend class Gui;
};

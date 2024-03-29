#include "button.hpp"
#include "gui.hpp"

extern "C" {
#include "theme.h"
}

Button::Button() {
    m_isActivated = false;
    m_isSelected = false;
}

void Button::draw() {
    // Offset calculation
    s32 resultX = position.first - m_gui->m_targetOffsetX;
    s32 resultY = position.second - m_gui->m_targetOffsetY;
    if (resultX + volume.first < 0 || resultY + volume.second < 0 || resultX > SCREEN_WIDTH || resultY > SCREEN_HEIGHT)
        return;
    s32 borderX = resultX - 5;
    s32 borderY = resultY - 5;
    if (m_isSelected) {
        m_gui->drawRectangled(borderX, borderY, volume.first + 10, volume.second + 10, m_isActivated ? currTheme.activatedColor : currTheme.highlightColor);
        m_gui->drawShadow(borderX, borderY, volume.first + 10, volume.second + 10);
    } else
        m_gui->drawShadow(resultX, resultY, volume.first, volume.second);

    m_gui->drawRectangled(resultX, resultY, volume.first, volume.second, currTheme.selectedButtonColor);

    drawAction(m_gui, resultX, resultY, &m_isActivated);

    if (!usableCondition())
        m_gui->drawRectangled(resultX, resultY, volume.first, volume.second, m_gui->makeColor(0x80, 0x80, 0x80, 0x80));
}

bool Button::onInput(u32 kdown) {
    if (!m_isSelected)
        return false;

    if (!m_isActivated) {
        if ((kdown & HidNpadButton_A) && activatable) {
            m_isActivated = true;
            kdown = 0;
        } else {
            if (kdown & HidNpadButton_AnyUp) m_gui->selectButton(adjacentButton[0]);
            if (kdown & HidNpadButton_AnyDown) m_gui->selectButton(adjacentButton[1]);
            if (kdown & HidNpadButton_AnyLeft) m_gui->selectButton(adjacentButton[2]);
            if (kdown & HidNpadButton_AnyRight) m_gui->selectButton(adjacentButton[3]);
            if (kdown & (HidNpadButton_AnyUp | HidNpadButton_AnyDown | HidNpadButton_AnyLeft | HidNpadButton_AnyRight)) return true;
        }
    }

    if (!usableCondition()) return false;

    inputAction(kdown, &m_isActivated);

    return false;
}

void Button::onTouch(HidTouchState &touch) {
    if (!usableCondition()) return;

    u16 resultX = position.first > m_gui->m_targetOffsetX ? position.first - m_gui->m_targetOffsetX : 0;
    u16 resultY = position.second > m_gui->m_targetOffsetY ? position.second - m_gui->m_targetOffsetY : 0;

    if (touch.x >= resultX && touch.x <= (resultX + volume.first) && touch.y >= resultY && touch.y <= (resultY + volume.second)) {
        if (m_isSelected) {
            if (activatable)
                m_isActivated = true;
            else
                inputAction(HidNpadButton_A, &m_isActivated);
            return;
        }

        m_gui->selectButtonByRef(this);
    }
}

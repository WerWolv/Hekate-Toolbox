#pragma once

#include "gui.hpp"

#include <vector>
#include <string>
#include <map>
#include "utils.hpp"
#include "SimpleIniParser.hpp"

class GuiHIDMitm : public Gui {
public:
    GuiHIDMitm();
    ~GuiHIDMitm() override;

    void draw() override;
    void onInput(u32 kdown) override;

private:
    simpleIniParser::Ini *m_configFile;
    std::map<std::string, std::string> m_hidConfig;
    s32 m_selectedButton = -1;
};

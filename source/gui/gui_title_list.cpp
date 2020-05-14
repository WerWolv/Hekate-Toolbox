#include "gui_title_list.hpp"
#include "button.hpp"
#include "utils.hpp"
#include <memory>
#include <string>
#include <cstring>
#include "gui_override_key.hpp"
#include "titleinfo.hpp"
#include "SimpleIniParser.hpp"
#include <cstdlib>

#define COLUMNS      6
#define ICON_SIZE    160
#define ICON_PADDING (ICON_SIZE + 14)
#define TEXT_BORDER  10
#define GRIDPOS      ((SCREEN_WIDTH - (COLUMNS * ICON_PADDING)) / 2)

static constexpr u64 availableApplets[]{
    AppletID::AppletController,
    AppletID::AppletMyPage,
    AppletID::AppletShop,
    AppletID::AppletPhotoViewer,
};

GuiTitleList::GuiTitleList() : Gui() {
    auto apps = DumpAllTitles(WidthHeight{ICON_SIZE, ICON_SIZE});
    int buttonIndex = 0;
    int buttonSelection = 0;

    size_t xOffset = 0, yOffset = 0;

    for (auto &&applet : availableApplets) {
        apps.insert(apps.begin(), std::make_shared<TitleIcon>(TitleIcon{.application_id = applet}));
    }

    for (auto &&app : apps) {

        auto appButton = Button();
        appButton.position = {GRIDPOS + xOffset, 194 + yOffset};
        appButton.volume = {ICON_SIZE, ICON_SIZE};
        appButton.drawAction = [app, buttonIndex](Gui *gui, u16 x, u16 y, bool *isActivated) {
            if (app != nullptr && app->application_id != 0) {

                auto appletName = GetAppletName(app->application_id);
                if (appletName == nullptr)
                    appletName = app->name;

                if (app->icon.get() != nullptr) {
                    gui->drawImage(x, y, ICON_SIZE, ICON_SIZE, app->icon.get(), ImageMode::IMAGE_MODE_RGBA32);
                } else {
                    gui->drawTextAligned(fontIcons, x + ICON_SIZE / 2, y + ICON_SIZE / 2 + 24, GetAppletColor(app->application_id), GetAppletIcon(app->application_id), ALIGNED_CENTER);
                }

                if (gui->getSelectedButtonIndex() == buttonIndex) {
                    u32 textWidth, textHeight;
                    gui->getTextDimensions(font20, appletName, &textWidth, &textHeight);
                    gui->drawRectangled((x + ICON_SIZE / 2 - textWidth / 2) - TEXT_BORDER * 2, (y - 32 - textHeight) - TEXT_BORDER, textWidth + TEXT_BORDER * 4, textHeight + TEXT_BORDER * 2, currTheme.overlayColor);
                    gui->drawTextAligned(font20, x + ICON_SIZE / 2, y - 36, currTheme.highlightTextColor, appletName, ALIGNED_CENTER);
                }

            } else {
                gui->drawTextAligned(fontHuge, x + ICON_SIZE / 2, y + ICON_SIZE / 2 + 48, currTheme.textColor, "\uE06B", ALIGNED_CENTER);
            }
        };
        appButton.inputAction = [&, app](u64 kdown, bool *isActivated) {
            if (kdown & KEY_A) {

                //convert title id to a hex string
                char buffer[17];
                sprintf(buffer, "%016lx", app->application_id);

                //write title id to config
                simpleIniParser::Ini *ini = parseOrCreateFileFixed(LOADER_INI);
                ini->findOrCreateSection(HBL_CONFIG, true, simpleIniParser::IniSectionType::Section)
                    ->findOrCreateFirstOption(OverrideKey::getOverrideProgramString(GuiOverrideKey::g_keyType), "")
                    ->value = buffer;

                ini->writeToFile(LOADER_INI);
                delete ini;
                Gui::g_nextGui = GUI_OVERRIDE_KEY;
            }
        };
        appButton.adjacentButton = {
            (buttonIndex - COLUMNS),                                         //UP
            (buttonIndex + COLUMNS),                                         //DOWN
            (buttonIndex % COLUMNS) != 0 ? (buttonIndex - 1) : -1,           //LEFT
            (buttonIndex % COLUMNS) != COLUMNS - 1 ? (buttonIndex + 1) : -1, //RIGHT
        };
        add(appButton);

        if (app->application_id == selectedAppID) {
            buttonSelection = buttonIndex;
        }

        xOffset += ICON_PADDING;
        if (xOffset >= COLUMNS * ICON_PADDING) {
            xOffset = 0;
            yOffset += ICON_PADDING;
        }
        buttonIndex++;
    }

    selectButton(buttonSelection);
    endInit();
}

GuiTitleList::~GuiTitleList() {}

void GuiTitleList::draw() {
    Gui::beginDraw();
    Gui::drawRectangle(0, 0, Gui::g_framebuffer_width, Gui::g_framebuffer_height, currTheme.backgroundColor);

    Gui::drawTextAligned(font24, Gui::g_framebuffer_width / 2 - m_targetOffsetX, 150 - m_targetOffsetY, currTheme.textColor, "Select the title you wish to override.", ALIGNED_CENTER);

    drawButtons();

    Gui::drawRectangle(0, 0, Gui::g_framebuffer_width, 88, currTheme.backgroundColor);
    Gui::drawRectangle(0, Gui::g_framebuffer_height - 73, Gui::g_framebuffer_width, 73, currTheme.backgroundColor);
    Gui::drawRectangle((u32)((Gui::g_framebuffer_width - 1220) / 2), 87, 1220, 1, currTheme.textColor);
    Gui::drawRectangle((u32)((Gui::g_framebuffer_width - 1220) / 2), Gui::g_framebuffer_height - 73, 1220, 1, currTheme.textColor);
    Gui::drawTextAligned(fontIcons, 70, 68, currTheme.textColor, "\uE130", ALIGNED_LEFT);
    Gui::drawTextAligned(font24, 70, 58, currTheme.textColor, "        Application override settings", ALIGNED_LEFT);
    Gui::drawTextAligned(font20, Gui::g_framebuffer_width - 50, Gui::g_framebuffer_height - 25, currTheme.textColor, "\uE0E1 Back     \uE0E0 OK", ALIGNED_RIGHT);
    Gui::endDraw();
}

void GuiTitleList::onInput(u32 kdown) {
    if (inputButtons(kdown)) return;

    if (kdown & KEY_B)
        Gui::g_nextGui = GUI_OVERRIDE_KEY;
}
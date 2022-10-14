#pragma once

#include <switch.h>
#include <cstring>
#include <unordered_map>
#include <memory>

#include "list_selector.hpp"
#include "message_box.hpp"
#include "button.hpp"

extern "C" {
#include "theme.h"
#include "types.h"
}

#include <ft2build.h>
#include FT_FREETYPE_H

#define FONT_FACES_MAX PlSharedFontType_Total

#define fontIcons 5
#define fontHuge  4
#define font24    3
#define font20    2
#define font14    1

#define SCREEN_WIDTH  1280
#define SCREEN_HEIGHT 720

enum gui_t {
    GUI_INVALID,
    GUI_MAIN,
    GUI_SM_SELECT,
    GUI_HEKATE,
    GUI_OVERRIDES_MENU,
    GUI_OVERRIDE_KEY,
    GUI_TITLE_LIST,
};

class Gui {
protected:
    u16 m_pageOffsetX = 0;
    u16 m_pageOffsetY = 0;
    u16 m_targetOffsetX = 0;
    u16 m_targetOffsetY = 0;
    const u16 m_pageLeftmostBoundary = 100;
    const u16 m_pageTopmostBoundary = 160;
    const u16 m_pageRightmostBoundary = SCREEN_WIDTH - 100;
    const u16 m_pageBottommostBoundary = SCREEN_HEIGHT - 160;
    bool m_scrollBlocked = false;

    std::vector<std::unique_ptr<Button>> m_buttons;
    friend class Button;

public:
    static inline enum gui_t g_nextGui = GUI_INVALID;

    u8 *framebuffer;
    static inline Framebuffer g_fb_obj;
    static inline u32 g_framebuffer_width = 1280;
    static inline u32 g_framebuffer_height = 720;
    static inline bool g_exitBlocked = false;

    static inline ListSelector *g_currListSelector = nullptr;
    static inline MessageBox *g_currMessageBox = nullptr;

public:
    Gui();
    virtual ~Gui();

    virtual void update();
    virtual void draw() {
        beginDraw();
        drawButtons();
        endDraw();
    }
    virtual void onInput(u32 kdown) {
        inputButtons(kdown);
    }
    virtual void onTouch(HidTouchState &touch) {
        touchButtons(touch);
    }
    virtual void onGesture(HidTouchState &startPosition, HidTouchState &endPosition){};
    s16 getSelectedButtonIndex();
    void selectButton(s16 buttonIndex);
    void selectButtonByRef(const Button *button);
    void add(Button *button);

    static void resizeImage(u8 *in, u8 *out, size_t src_width, size_t src_height, size_t dest_width, size_t dest_height);
    static std::vector<std::string> split(const std::string &s, const char &c);
    inline u8 blendColor(u32 src, u32 dst, u8 alpha);
    color_t makeColor(u8 r, u8 g, u8 b, u8 a);
    void drawRectangle(s16 x, s16 y, s16 w, s16 h, color_t color);
    void drawRectangled(s16 x, s16 y, s16 w, s16 h, color_t color);

    bool fontInit();
    void fontExit();

    void drawText(u32 font, s16 x, s16 y, color_t clr, const char *text);
    void drawTextAligned(u32 font, s16 x, s16 y, color_t clr, const char *text, TextAlignment alignment);
    void drawTextTruncate(u32 font, s16 x, s16 y, color_t clr, const char *text, u32 max_width, const char *end_text);
    void getTextDimensions(u32 font, const char *text, u32 *width_out, u32 *height_out);

    void drawImage(s16 x, s16 y, s16 width, s16 height, const u8 *image, ImageMode mode);
    void drawImage(s16 x, s16 y, s16 startx, s16 starty, s16 width, s16 height, const u8 *image, ImageMode mode);
    void drawShadow(s16 x, s16 y, s16 width, s16 height);

protected:
    void endInit() {
        m_targetOffsetX = m_pageOffsetX;
        m_targetOffsetY = m_pageOffsetY;
    }
    void beginDraw();
    void endDraw();
    void drawButtons() {
        for (auto &btn : m_buttons)
            btn->draw();
    };

    bool inputButtons(u32 kdown) {
        for (auto &btn : m_buttons) {
            if (btn->isSelected())
                if (btn->onInput(kdown)) return true;
        }
        return false;
    };

    void touchButtons(HidTouchState &touch) {
        for (auto &btn : m_buttons) {
            btn->onTouch(touch);
        }
    };

private:
    FT_Error m_fontLibret, m_fontFacesRet[FONT_FACES_MAX];
    FT_Library m_fontLibrary;
    FT_Face m_fontFaces[FONT_FACES_MAX];
    FT_Face m_fontLastUsedFace;
    s32 m_fontFacesTotal;

    std::unordered_map<size_t, std::pair<u16, u16>> m_stringDimensions;

    void drawText_(u32 font, s16 x, s16 y, color_t clr, const char *text, s32 max_width, const char *end_text);
    inline void draw4PixelsRaw(s16 x, s16 y, color_t clr);
    inline bool fontLoadGlyph(glyph_t *glyph, u32 font, u32 codepoint);
    void drawGlyph(s16 x, s16 y, color_t clr, const glyph_t *glyph);
    bool setFontType(u32 font);
    inline u8 decodeByte(const char **ptr);
    inline s8 decodeUTF8Cont(const char **ptr);
    inline u32 decodeUTF8(const char **ptr);
    inline void drawPixel(s16 x, s16 y, color_t clr);
};

#define COLOR_WHITE makeColor(255, 255, 255, 255)
#define COLOR_BLACK makeColor(0, 0, 0, 255)
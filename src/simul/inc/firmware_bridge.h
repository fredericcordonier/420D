#pragma once
#include <array>
#include <string>

constexpr int LCD_LINES = 6;
constexpr int LCD_COLS  = 30;

enum class Button {
    MENU,
    DP,
    INFO,
    PLAY,
    TRASH,
    UP,
    DOWN,
    LEFT,
    RIGHT,
    SET,
    AV,
    DRIVE,
    WHEEL_L,
    WHEEL_R,
    SHUTTER_HALF,
    SHUTTER_FULL,
    ZOOM_IN,
    ZOOM_OUT,
    FACE_SENSOR,
};

class FirmwareBridge {
public:
    virtual ~FirmwareBridge() = default;

    virtual void onButton(Button btn) = 0;
    virtual void redraw(void) = 0;
    virtual void setLCD(const std::array<std::string, LCD_LINES>& lines) = 0;
    virtual void setFaceSensor(bool active) = 0;
};

#pragma once

#include <gtk/gtk.h>
#include <array>
#include <string>
#include "firmware_bridge.h"

class EOS400DWindow {
public:
    EOS400DWindow(FirmwareBridge& fw);
    void show();

    void updateLCD(const std::array<std::string, LCD_LINES>& lines);

private:
    FirmwareBridge& firmware;

    GtkWidget* window;
    GtkWidget* lcdView;
    GtkTextBuffer* lcdBuffer;
    GtkWidget* faceSensorToggle;
    GtkWidget* makeButton(const char* label, Button btn);
    static void onButtonClicked(GtkWidget*, gpointer user_data);
    static void onFaceSensorToggled(GtkWidget* widget, gpointer user_data);
    bool isFaceSensorActive() const;
};

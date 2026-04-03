#pragma once

#include <array>
#include <string>
#include "button.h"
#include "keypad.h"
#include "firmware_bridge.h"

#define GTK_GRID(grid) (0)
#define gtk_grid_attach(a, b,  c, d, e, f)   (b)
#define g_print printf
void gtk_init(int *argc, char **argv[]);
void gtk_main(void);

typedef void *gpointer;

class MyButton {

};

class GtkWidget {
public:

};

class GtkTextBuffer {

};

typedef void callback(GtkWidget *widget, gpointer user_data);

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
    void makeButton(char btn_key, const char* label, button_t btn);
    static void onButtonClicked(KeypadInput ki);
    // static void onFaceSensorToggled(GtkWidget* widget, gpointer user_data);
    bool isFaceSensorActive() const;
};


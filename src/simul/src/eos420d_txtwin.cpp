#include <iostream>
#include "button.h"
#include "eos420d.h"
#include "eos420d_txtwin.h"

typedef struct {
    char          key_code;
    const char   *button_text;
    button_t      button_code;
} button_config;

const button_config txt_ui_buttons[] = {
    {'p', "DP", BUTTON_DP},
    {'a', "AV", BUTTON_AV},
    {'s', "Shoot Mode", BUTTON_DRIVE},
    {'-', "ZOOM OUT", BUTTON_ZOOM_OUT},
    {'+', "ZOOM IN", BUTTON_ZOOM_IN},
    {'w', "WHEEL LEFT", BUTTON_WHEEL_LEFT},
    {'e', "WHEEL RIGHT", BUTTON_WHEEL_RIGHT},
    {'8', "UP", BUTTON_UP},
    {'4', "LEFT", BUTTON_LEFT},
    {'5', "SET", BUTTON_SET},
    {'6', "RIGHT", BUTTON_RIGHT},
    {'2', "DOWN", BUTTON_DOWN},
    // Key to quit the application
    {'q', "Quit", BUTTON_COUNT},
};

EOS400DWindow::EOS400DWindow(FirmwareBridge& fw)
    : firmware(fw)
{

    for (int i = 0; i < sizeof(txt_ui_buttons) / sizeof(button_config); i++) {
        makeButton(txt_ui_buttons[i].key_code, txt_ui_buttons[i].button_text, txt_ui_buttons[i].button_code);
    }

    // faceSensorToggle = gtk_toggle_button_new_with_label("Face Sensor");
    // g_signal_connect(faceSensorToggle, "toggled",
    //                 G_CALLBACK(EOS400DWindow::onFaceSensorToggled),
    //                 this);

}

// void EOS400DWindow::onFaceSensorToggled(GtkWidget* widget, gpointer user_data)
// {
    // EOS400DWindow* ui = static_cast<EOS400DWindow*>(user_data);
    // bool active = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ui->faceSensorToggle));

    // // Mise à jour du firmware bridge
    // ui->firmware.setFaceSensor(active);

    // g_print("Face sensor is %s\n", active ? "ACTIVE" : "INACTIVE");
// }

void EOS400DWindow::makeButton(char btn_char, const char* label, button_t btn)
{
    my_camera.keypad.add_button(btn_char, btn);
}

void EOS400DWindow::updateLCD(const std::array<std::string, LCD_LINES>& lines)
{
    for (int i = 0; i < sizeof(txt_ui_buttons) / sizeof(button_config); i++) {
        std::cout << txt_ui_buttons[i].key_code << ':' << txt_ui_buttons[i].button_text << " | ";
    }
    std::cout << std::endl;
    for (const auto& line : lines) {
        std::cout << line.c_str() << std::endl;
    }
}

void EOS400DWindow::show()
{
}

bool EOS400DWindow::isFaceSensorActive() const {
        // return gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(faceSensorToggle));
    return false;
}

void gtk_init(int *argc, char **argv[])
{

}

void gtk_main(void)
{
    KeypadInput key_pressed;
    do {
        key_pressed = my_camera.keypad.read_kp();
        std::cout << key_pressed.button << std::endl;
        if (key_pressed.button == BUTTON_NONE) {
            my_camera.hw_control(key_pressed.key);
        }
        if (key_pressed.button != BUTTON_COUNT) {
            my_camera.button_received(key_pressed.button);
        }
    }
    while (key_pressed.button != BUTTON_COUNT);
}

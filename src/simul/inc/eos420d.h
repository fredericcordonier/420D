#ifndef EOS420D_H
#define EOS420D_H

#include <queue>
#include <string>
#include "main.h"
#include "file_system.h"
#include "keypad.h"
#include "firmware_bridge.h"
#include "eos420d_window.h"

#define COL_DISPLAY  20

class Display {
    public:
    Display();
    std::string get_display_line(const int item);
    void redraw();
    void clear();
    void print_line(const int item, const char *data);
    void highlight_line(const int item, const int enable);
    void set_fw(FirmwareBridge *fw);
    private:
    FirmwareBridge *firmware;
    std::string display[LCD_LINES];
    char highlight[LCD_LINES];
};

class Intercom {
    public:
    void send(int message, int length, int param);
    private:
};

class EOS420D {
    public:
    EOS420D();
    FileSystem flash_420d;
    Intercom   intercom_420d;
    Display    display;
    Keypad     keypad;
    void       post_message(action_t pMessage);
    void       hw_control(const Button button);
    void       button_received(KeypadInput ki);
    private:
    std::queue<action_t> message_queue;
    void display_af_patterns(void);
};

extern EOS420D my_camera;

#endif // EOS420D_H

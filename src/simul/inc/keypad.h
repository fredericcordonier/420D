#ifndef KEYPAD_H
#define KEYPAD_H

#include <map>
#include "firmware_bridge.h"
#include "button.h"

class KeypadInput {
    public:
    KeypadInput(char c, button_t b) {key=c; button=b;}
    KeypadInput() {key='q'; button=BUTTON_COUNT;}
    char key;
    button_t button;
};

class Keypad {
    public:
    Keypad();
    void add_button(char btn_key, button_t btn);
    KeypadInput read_kp();
    private:
    std::map<char, button_t>map_table;
};

#endif // KEYPAD_H

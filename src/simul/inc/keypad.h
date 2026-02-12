#ifndef KEYPAD_H
#define KEYPAD_H

#include <map>
#include "firmware_bridge.h"
#include "button.h"

class KeypadInput {
    public:
    KeypadInput(Button c, button_t b) {key=c; button=b;}
    Button key;
    button_t button;
};

class Keypad {
    public:
    Keypad();
    // KeypadInput read_kp();
    private:
    std::map<char, button_t>map_table;
};

#endif // KEYPAD_H

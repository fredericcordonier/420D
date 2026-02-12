#include <map>
#include <iostream>
#include "keypad.h"
#include "button.h"

Keypad::Keypad() {
	map_table = {
		{'a', BUTTON_AV},
		{'e', BUTTON_WHEEL_RIGHT},
		{'i', BUTTON_UP},
		{'j', BUTTON_LEFT},
		{'k', BUTTON_SET},
		{'l', BUTTON_RIGHT},
		{'m', BUTTON_DOWN},
		{'p', BUTTON_DP},
		{'q', BUTTON_COUNT},
		{'r', BUTTON_RELEASE},
		{'u', BUTTON_ZOOM_OUT},
		{'w', BUTTON_WHEEL_LEFT},
		{'z', BUTTON_ZOOM_IN}
	};
	std::cout << "Keypad constructed" << std::endl;
		// {0, BUTTON_NONE}
}

// KeypadInput Keypad::read_kp() {
    // char input;
    // std::cout << "Press a key: ";
    // std::cin >> input;
	// std::map<char, button_t>::iterator it = map_table.find(input);
	// if (it != map_table.end()) {
	// 	return KeypadInput(input, it->second);
	// }
	// else {
	// 	return KeypadInput(input, BUTTON_NONE);
	// }
// }

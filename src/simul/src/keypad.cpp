#include <map>
#include <iostream>
#include "keypad.h"
#include "button.h"

Keypad::Keypad() {
	map_table = {
	};
	std::cout << "Keypad constructed" << std::endl;
		// {0, BUTTON_NONE}
}

void Keypad::add_button(char btn_key, button_t btn)
{
	map_table.emplace(std::make_pair(btn_key, btn));
}

KeypadInput Keypad::read_kp() {
    char input;
    std::cout << "Press a key: ";
    std::cin >> input;
	std::map<char, button_t>::iterator it = map_table.find(input);
	if (it != map_table.end()) {
		return KeypadInput(input, it->second);
	}
	else {
		return KeypadInput(input, BUTTON_NONE);
	}
}

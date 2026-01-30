#include <iostream>
#include <firmware/eventproc.h>

extern "C" {

int eventproc_RiseEvent(const char *event) {
    std::cout << "Rise event" << std::endl;
    return 1;
}

// LED management

long eventproc_EdLedOn(void) {
    std::cout << "Led on" << std::endl;
    return 1;
}

long eventproc_EdLedOff(void) {
    std::cout << "Led off" << std::endl;
    return 1;
}

long eventproc_EdLedBlink(void) {
    std::cout << "Led blink" << std::endl;
    return 1;
}

// Main display

int eventproc_PrintICUInfo(void) {
    std::cout << "PrintICUInfo" << std::endl;
    return 1;
}

// Shutter release

int eventproc_Release(void) {
    std::cout << "Shutter release" << std::endl;
    return 1;
}

// Remote

int eventproc_RemOn(void) {
    std::cout << "Remote on" << std::endl;
    return 1;
}
int eventproc_RemOff(void) {
    std::cout << "Remote off" << std::endl;
    return 1;
}

// int remote_on() {
//     return eventproc_RemOn();
// }

// int remote_off() {
//     return eventproc_RemOff();
// }

}

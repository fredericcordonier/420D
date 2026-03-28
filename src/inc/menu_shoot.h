
#pragma once

/**
 * \file menu_dlg.h Implementation of various dialogs displayed out of the 420D
 * menu
 */
#include "settings.h"

enum {
    MENUPAGE_SHOOT_MODE_INTERVAl,
    MENUPAGE_SHOOT_MODE_BRAMP,
    MENUPAGE_SHOOT_MODE_HANDWAVE,
    MENUPAGE_SHOOT_MODE_TIMER,
    MENUPAGE_SHOOT_MODE_LEXP,
    MENUPAGE_SHOOT_MODE_TIMELAPSE,
    MENUPAGE_SHOOT_MODE_COUNT,
    MENUPAGE_SHOOT_MODE_FIRST = 0,
    MENUPAGE_SHOOT_MODE_LAST  = MENUPAGE_SHOOT_MODE_COUNT - 1
};

void menu_shoot_mode_start(void);

#ifndef MENU_DLG_H_
#define MENU_DLG_H_

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
    MENUPAGE_SHOOT_MODE_COUNT,
    MENUPAGE_SHOOT_MODE_FIRST = 0,
    MENUPAGE_SHOOT_MODE_LAST  = MENUPAGE_SHOOT_MODE_COUNT - 1
};

typedef struct {
    int temps[CCT_COUNT];
    char names[CCT_COUNT][LP_MAX_WORD];
} named_temps_t;

extern named_temps_t named_temps;
void menu_named_temps_start(void);
void menu_shoot_mode_start(void);

#endif /* MENU_DLG_H_ */

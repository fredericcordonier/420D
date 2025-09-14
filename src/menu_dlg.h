#ifndef MENU_DLG_H_
#define MENU_DLG_H_

/**
 * \file menu_dlg.h Implementation of various dialogs displayed out of the 420D menu
 */
#include "settings.h"

typedef struct {
    int initd;
    int temps[CCT_COUNT];
    char names[CCT_COUNT][LP_MAX_WORD];
} named_temps_t;

extern named_temps_t named_temps;
void menu_named_temps_start(void);

#endif /* MENU_DLG_H_ */

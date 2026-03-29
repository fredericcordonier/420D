/**
 * @file menu_temps.h
 * @author Frédéric Cordonier
 * @brief Menu for named temperaturs.
 * @version 0.1
 * @date 2026-03-28
 *
 * @copyright Copyright (c) 2026
 *
 * Menu Named Temperatures:
 *
 * List of named temperatures read from the file A:/420D/NAMED_TEMPS.CSV
 * The structure of this file is:
 * <Named temerature>;<defaul_temp_value>;<min_temp_value>;<max_temp_value>
 * There can be an arbitrary number of lines, which will be displayed as pages
 * of 5 items.
 * The color temperatures are never precise, this is why it is possible to
 * specify min-max values; initially, the value is displayed, the user can
 * change the value between min and max. When pressing SET, the dislayed value
 * is set as custom color temperature.
 *
 * The named temperatures menu is displayed when pressing DP while in the WB
 * selection dialog.
 */

#pragma once

#include "settings.h"
#include "languages.h"

typedef struct {
    int temps[CCT_COUNT];
    char names[CCT_COUNT][LP_MAX_WORD];
    int named_temps_top_of_page_line;
} named_temps_t;

extern named_temps_t s_named_temps;
void menu_named_temps_start(void);

/**
 * @file menu_dlg.c
 * @author Frédéric Cordonier
 * @brief Menu for various dialog menus.
 * @version 0.1
 * @date 2024-04-07
 *
 * @copyright Copyright (c) 2024
 *
 * Menu Named Temperatures:
 *
 * List of named temperatures read from the file A:/420D/NAMED_TEMPS.CSV
 * The structure of this file is:
 * <Named temerature>;<temp_value>;<min_temp_value>;<max_temp_value>
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

#include "firmware/gui.h"

#include "debug.h"
#include "firmware.h"
#include "firmware/camera.h"
#include "intercom.h"
#include "macros.h"
#include "main.h"
#include "utils.h"

#include "csv.h"
#include "float.h"
#include "languages.h"
#include "menu.h"
#include "menu_developer.h"
#include "menuitem.h"
#include "menupage.h"
#include "settings.h" // named_temps could be relocated here...?
#include "utils.h"    // beep function

#include "menu_dlg.h"
#include "scripts.h"

#define NAMED_TEMPS_FILENAME "NAMED_TEMPS.CSV"

/**
 * Structure containing the named temperatures being displayed.
 */
named_temps_t named_temps;

static void named_temps_init(menu_t *menu);
static void named_temps_pgdown(menu_t *menu);
static void named_temps_pgup(menu_t *menu);
static void apply_named_temp(const menuitem_t *item);
static void named_temps_up(menu_t *menu);
static void named_temps_down(menu_t *menu);

static void named_temps_init(menu_t *menu);
static void named_temp_save(menu_t *menu);
static void apply_named_temp(const menuitem_t *item);
// methods managing the paging of named temperatures:
static void named_temps_pgdown(menu_t *menu);
static void named_temps_pgup(menu_t *menu);
static void named_temps_down(menu_t *menu);
static void named_temps_up(menu_t *menu);

// Named temps positions are limited to the size of the menu. They are
// dynamically loaded per page from the csv file.
static menuitem_t named_temps_items[MENU_HEIGHT] = {
    MENUITEM_NAMEDCT(0, named_temps.names[0], &named_temps.temps[0],
                     apply_named_temp),
    MENUITEM_NAMEDCT(1, named_temps.names[1], &named_temps.temps[1],
                     apply_named_temp),
    MENUITEM_NAMEDCT(2, named_temps.names[2], &named_temps.temps[2],
                     apply_named_temp),
    MENUITEM_NAMEDCT(3, named_temps.names[3], &named_temps.temps[3],
                     apply_named_temp),
    MENUITEM_NAMEDCT(4, named_temps.names[4], &named_temps.temps[4],
                     apply_named_temp),
};

/**
 * Page for named temps. In actions, specific methods for page-up and page-down
 */
static menupage_t named_temps_page = {
    name : LP_WORD(L_S_NAMED_TEMPS),
    items : LIST(named_temps_items),
    actions : {
        [MENU_EVENT_OPEN] = named_temps_init,
        [MENU_EVENT_IN] = named_temps_pgdown,
        [MENU_EVENT_OUT] = named_temps_pgup,
        [MENU_EVENT_UP] = named_temps_up,
        [MENU_EVENT_DOWN] = named_temps_down,
        [MENU_EVENT_PREV] = named_temps_up,
        [MENU_EVENT_NEXT] = named_temps_down,
        [MENU_EVENT_SAVE] = named_temp_save,
    }
};

enum {
    MENUPAGE_NAMED_TEMPS,
};

static menupage_t *menu_dialog_pages[] = {
    [MENUPAGE_NAMED_TEMPS] = &named_temps_page,
};

static menu_t menu_dialogs = {
    pages : LIST(menu_dialog_pages),
    actions : {
        [MENU_EVENT_SET] = menu_set,
        [MENU_EVENT_UP] = menupage_up,
        [MENU_EVENT_DOWN] = menupage_down,
        [MENU_EVENT_PREV] = menupage_up,
        [MENU_EVENT_NEXT] = menupage_down,
        [MENU_EVENT_LEFT] = menu_left,
        [MENU_EVENT_RIGHT] = menu_right,
        [MENU_EVENT_DISPLAY] = menupage_display,
        [MENU_EVENT_REFRESH] = menupage_refresh,
        [MENU_EVENT_FINISH] = menu_finish,
        [MENU_EVENT_TRASH] = menupage_developer_start,
        // Named temps: nothing to save, selected temp will be saved by OFW
        //        [MENU_EVENT_SAVE]    = menu_dlg_save,
        //        [MENU_EVENT_REFRESH] = menupage_refresh,
        //        [MENU_EVENT_IN]      = named_temps_pgdown,
        //        [MENU_EVENT_OUT]     = named_temps_pgup,
    }
};

/**
 * Start menu_dialog menu on the page given by current pos.
 * @param current_pos
 */
static void menu_dialogs_start(int current_pos) {
    menu_dialogs.current_posn = current_pos;
    menu_create(&menu_dialogs);
}

/**
 * Start the named_temps dialog.
 */
void menu_named_temps_start(void) {
    // Init named temps from file is done through the MENU_EVENT_OPEN event
    named_temps_page.current_line = settings.named_temps_cur_line;
    named_temps_page.current_posn = settings.named_temps_cur_line;
    menu_dialogs.color = MENU_COLOR_DARK_BLUE;
    menu_dialogs_start(MENUPAGE_NAMED_TEMPS);
}

/**
 * @brief Struct used in csv parsing handler to get info from.
 *
 */
typedef struct {
    int nb_values;      /**< Number of temp values retrieved */
    int last_line_read; /**< Last line number handled by callback */
} named_temps_parsing;

/**
 * @brief extract integer value from its string represenation
 *
 * @param pc_x_val pointer to string
 * @return int integer value
 */
static int atoi(const char *pc_x_val) {
    int i_l_val = 0;
    while (*pc_x_val) {
        if (*pc_x_val >= '0' && *pc_x_val <= '9') {
            i_l_val = 10 * i_l_val + (*pc_x_val - '0');
        } else {
            break;
        }
        pc_x_val++;
    }
    return i_l_val;
}

/**
 * @brief Callback to handle a named temp read from file.
 *
 * @param user   Data given to callback from original caller.
 * @param lineno Line number in file.
 * @param name   Name of temp.
 * @param value  Value of temp.
 * @return int   Continue (1) or stop (0).
 */
static int named_temps_handler(void *user, int lineno, const char *name,
                               const char *pc_x_value, char *pc_x_min_value,
                               char *pc_x_max_value) {
    int temp_index = ((named_temps_parsing *)user)->nb_values;
    if (temp_index < MENU_HEIGHT) {
        // Get values as integers
        int i_l_value = atoi(pc_x_value);
        int i_l_min_value = atoi(pc_x_min_value);
        int i_l_max_value = atoi(pc_x_max_value);
        // Copy found temp name and value into array
        strncpy0(named_temps.names[temp_index], name, LP_MAX_WORD);
        named_temps.temps[temp_index] = i_l_value;
        // Set boundaries of temp item so that the user may adjust them
        named_temps_items[temp_index].parm.menuitem_int.min = i_l_min_value;
        named_temps_items[temp_index].parm.menuitem_int.max = i_l_max_value;
        // Increment the number of temp values found
        ((named_temps_parsing *)user)->nb_values++;
        ((named_temps_parsing *)user)->last_line_read = lineno;
        return 1;
    }
    return 0;
}

/**
 * @brief Initialize the named temps.
 *
 * Read from CSV file.
 *
 * @param menu The menu we need to initialize.
 */
static void named_temps_init(menu_t *menu) {
    named_temps_parsing s_l_nt_parsed;
    s_l_nt_parsed.nb_values = 0;
    if (!named_temps.initd) {
        // Launch named temps from csv file
        csv_parse(MKPATH_NEW(NAMED_TEMPS_FILENAME),
                  settings.named_temps_top_of_page_line, named_temps_handler,
                  &s_l_nt_parsed);

        // Change the size of items to match what was read from file
        named_temps_page.items.size = s_l_nt_parsed.nb_values;
        named_temps.initd = TRUE;
    }
}

/**
 * @brief Save the settings parameters.
 *
 * @param menu
 */
static void named_temp_save(menu_t *menu) {
    debug_log("curline: %d, top_of_page_line: %d",
              named_temps_page.current_line,
              settings.named_temps_top_of_page_line);
    settings.named_temps_cur_line = named_temps_page.current_line;
    enqueue_action(settings_write);
    debug_log("named_temp_save end");
}

/**
 * @brief Upon page down, the next values shall be read from file.
 *
 * @param menu ignored
 */
static void named_temps_pgdown(menu_t *menu) {
    if (named_temps_page.items.size == MENU_HEIGHT) {
        // Page was completely filled -> maybe some more elements in file
        named_temps_parsing s_l_nt_parsed;
        s_l_nt_parsed.nb_values = 0;
        settings.named_temps_top_of_page_line += MENU_HEIGHT;
        // Launch named temps from csv file
        csv_parse(MKPATH_NEW(NAMED_TEMPS_FILENAME),
                  settings.named_temps_top_of_page_line, named_temps_handler,
                  &s_l_nt_parsed);
        if (s_l_nt_parsed.nb_values > 0) {
            // Do something only if there were remaining items in csv file
            named_temps_page.items.size = s_l_nt_parsed.nb_values;
            named_temps_page.current_line = 0;
            named_temps_page.current_posn = 0;
        } else {
            // Nothing found, keep the same top_of_page_line
            settings.named_temps_top_of_page_line -= MENU_HEIGHT;
            // And put cursor at last display line
            named_temps_page.current_line = MENU_HEIGHT - 1;
            named_temps_page.current_posn = named_temps_page.current_line;
        }
    } else {
        // Did not change page, but user friendly to put cursor at the end of
        // the page
        named_temps_page.current_line = named_temps_page.items.size - 1;
        named_temps_page.current_posn = named_temps_page.current_line;
    }
    menu_event_display();
}

/**
 * @brief Load previous named temps. Called either from up or pgup,
 * position of cursor will depend.
 *
 * @return int 1 if elements loaded, 0 otherwise.
 */
static int named_temps_load_pgup(void) {
    // get the previous named temps if any and send display event
    if (settings.named_temps_top_of_page_line >= MENU_HEIGHT) {
        // We are not on first page, go back one page
        settings.named_temps_top_of_page_line -= MENU_HEIGHT;

        named_temps_parsing s_l_nt_parsed;
        s_l_nt_parsed.nb_values = 0;
        // Launch named temps from csv file
        csv_parse(MKPATH_NEW(NAMED_TEMPS_FILENAME),
                  settings.named_temps_top_of_page_line, named_temps_handler,
                  &s_l_nt_parsed);

        if (s_l_nt_parsed.nb_values != 0) {
            // Change the size of items to match what was read from file
            named_temps_page.items.size = s_l_nt_parsed.nb_values;
            return 1;
        } else {
            // if no other item found, re-set the top of page line
            settings.named_temps_top_of_page_line += MENU_HEIGHT;
            return 0;
        }
    }
    return 0;
}

/**
 * @brief pagination pgup.
 *
 * If cursor is on top of screen, go to previous page, otherwise just go to the
 * top.
 * @param menu ignored
 */
static void named_temps_pgup(menu_t *menu) {
    if (named_temps_page.current_line == 0) {
        named_temps_load_pgup();
    }
    named_temps_page.current_line = 0;
    named_temps_page.current_posn = 0;
    menu_event_display();
}

/**
 * @brief Up key.
 *
 * @param menu unused
 */
static void named_temps_up(menu_t *menu) {
    if (named_temps_page.current_line != 0) {
        // We are not on first line, so just standard move up
        menupage_up(menu);
    } else {
        // Will change page only if we are not in the first "page" of the csv
        // file
        if (named_temps_load_pgup()) {
            // Items were loaded, place cursor at the bottom of the page
            named_temps_page.current_line = MENU_HEIGHT - 1;
            named_temps_page.current_posn = MENU_HEIGHT - 1;
            menu_event_display();
        }
    }
}

static void named_temps_down(menu_t *menu) {
    if (named_temps_page.items.size == MENU_HEIGHT) {
        // Page is full
        if (named_temps_page.current_line < MENU_HEIGHT - 1) {
            // Down only if not at last screen line
            menupage_down(menu);
        } else {
            // As page is full, maybe some more elements in next page
            named_temps_pgdown(menu);
        }
    } else if (named_temps_page.items.size >
               named_temps_page.current_line + 1) {
        // Page is partially filled, down only if not at last element, and don't
        // page down (we were already at end of file).
        menupage_down(menu);
    }
}

static void apply_named_temp(const menuitem_t *item) {
    send_to_intercom(IC_SET_WB, WB_MODE_COLORTEMP);
    send_to_intercom(IC_SET_COLOR_TEMP, *item->parm.menuitem_int.value);
    beep();
    menu_close();
}

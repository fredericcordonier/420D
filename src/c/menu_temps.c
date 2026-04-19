/**
 * @file menu_temps.c
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

#include "firmware/gui.h"
#include <string.h>
#include <stdlib.h>

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
#include "persist.h"
#include "utils.h"    // beep function

#include "menu_temps.h"
#include "scripts.h"

#define NAMED_TEMPS_FILENAME "NAMED_TEMPS.CSV"

/**
 * Structure containing the named temperatures being displayed.
 */
named_temps_t s_named_temps;

static void get_namedtemps_file_name(char *pc_x_buf);

static void named_temps_init(menu_t *ps_c_menu);
static void named_temps_pgdown(menu_t *ps_c_menu);
static void named_temps_pgup(menu_t *ps_c_menu);
static void apply_named_temp(const menuitem_t *ps_c_item);
static void named_temps_up(menu_t *ps_c_menu);
static void named_temps_down(menu_t *ps_c_menu);
static void named_temp_save(menu_t *ps_c_menu);
static void apply_named_temp(const menuitem_t *ps_c_item);

// Named temps positions are limited to the size of the menu. They are
// dynamically loaded per page from the csv file.
static menuitem_t as_named_temps_items[MENU_HEIGHT] = {
    MENUITEM_NAMEDCT(0, s_named_temps.names[0], &s_named_temps.temps[0],
                     apply_named_temp),
    MENUITEM_NAMEDCT(1, s_named_temps.names[1], &s_named_temps.temps[1],
                     apply_named_temp),
    MENUITEM_NAMEDCT(2, s_named_temps.names[2], &s_named_temps.temps[2],
                     apply_named_temp),
    MENUITEM_NAMEDCT(3, s_named_temps.names[3], &s_named_temps.temps[3],
                     apply_named_temp),
    MENUITEM_NAMEDCT(4, s_named_temps.names[4], &s_named_temps.temps[4],
                     apply_named_temp),
};

/**
 * Page for named temps. In actions, specific methods for page-up and page-down
 */
static menupage_t s_named_temps_page = {
    .name = LP_WORD(L_S_NAMED_TEMPS),
    .items = LIST(as_named_temps_items),
    .actions = (menuaction_spec_t[]) {
        {MENU_EVENT_OPEN,  named_temps_init},
        {MENU_EVENT_IN, named_temps_pgdown},
        {MENU_EVENT_OUT, named_temps_pgup},
        {MENU_EVENT_UP, named_temps_up},
        {MENU_EVENT_DOWN, named_temps_down},
        {MENU_EVENT_PREV, named_temps_up},
        {MENU_EVENT_NEXT, named_temps_down},
        {MENU_EVENT_SAVE, named_temp_save},
        {MENU_EVENT_SENTINEL, NULL}             // End marker
    }
};

static menupage_t *as_named_temps_pages[] = {
    &s_named_temps_page,
};

static menu_t s_menu_named_temps = {
    .pages = LIST(as_named_temps_pages),
    .actions = (menuaction_spec_t[]) {
        {MENU_EVENT_SET, menu_set},
        {MENU_EVENT_DISPLAY, menupage_display},
        {MENU_EVENT_REFRESH, menupage_refresh},
        {MENU_EVENT_FINISH, menu_finish},
        {MENU_EVENT_TRASH, menupage_developer_start},
        {MENU_EVENT_SENTINEL, NULL}             // End marker
    }
};

/**
 * Start the named_temps dialog.
 */
void menu_named_temps_start(void) {
    // Init named temps from file is done through the MENU_EVENT_OPEN event
    s_named_temps_page.current_line = persist.named_temps_cur_line;
    s_named_temps_page.current_posn = persist.named_temps_cur_line;
    s_menu_named_temps.color = MENU_COLOR_DARK_BLUE;
    s_menu_named_temps.current_posn = 0;
    menu_create(&s_menu_named_temps);
}

/**
 * @brief Struct used in csv parsing handler to get info from.
 *
 */
typedef struct {
    int i_nb_values;      /**< Number of temp values retrieved */
    int i_last_line_read; /**< Last line number handled by callback */
} named_temps_parsing_t;

/**
 * @brief Callback to handle a named temp read from file.
 *
 * @param user   Data given to callback from original caller.
 * @param lineno Line number in file.
 * @param name   Name of temp.
 * @param value  Value of temp.
 * @return int   Continue (1) or stop (0).
 */
static int i_named_temps_handler(void *p_x_user, int i_x_lineno, const char *pc_x_name,
                                const char *pc_x_value, char *pc_x_min_value,
                                char *pc_x_max_value) {
    int temp_index = ((named_temps_parsing_t *)p_x_user)->i_nb_values;
    if (temp_index < MENU_HEIGHT) {
        // Get values as integers
        int i_l_value = atoi(pc_x_value);
        int i_l_min_value = atoi(pc_x_min_value);
        int i_l_max_value = atoi(pc_x_max_value);
        // Copy found temp name and value into array
        strncpy0(s_named_temps.names[temp_index], pc_x_name, LP_MAX_WORD);
        s_named_temps.temps[temp_index] = i_l_value;
        // Set boundaries of temp item so that the user may adjust them
        as_named_temps_items[temp_index].parm.menuitem_int.min =
            MIN(i_l_min_value, i_l_value);
        as_named_temps_items[temp_index].parm.menuitem_int.max =
            MAX(i_l_max_value, i_l_value);
        // Increment the number of temp values found
        ((named_temps_parsing_t *)p_x_user)->i_nb_values++;
        ((named_temps_parsing_t *)p_x_user)->i_last_line_read = i_x_lineno;
        return 1;
    }
    return 0;
}

/** Get the named temps file name according to camera language.
 *
 * The file shall be in a sub-folder of 420D having the name of the language.
 */
static void get_namedtemps_file_name(char *pc_x_buf) {
    // Launch named temps from csv file
    strcpy(pc_x_buf, FOLDER_PATH);
    GetLanguageStr(DPData.language, pc_x_buf + strlen(FOLDER_PATH));
    stoupper(pc_x_buf); // convert to upper case
    strcpy(pc_x_buf + strlen(pc_x_buf), "/" NAMED_TEMPS_FILENAME);
}

/**
 * @brief Initialize the named temps.
 *
 * Read from CSV file.
 *
 * @param ps_c_menu The menu we need to initialize.
 */
static void named_temps_init(menu_t *ps_c_menu) {
    named_temps_parsing_t s_l_nt_parsed;

    // Get current values of named temps menu
    s_named_temps.named_temps_top_of_page_line = persist.named_temps_top_of_page_line;

    s_l_nt_parsed.i_nb_values = 0;
    char ac_l_folder_lang[LP_MAX_WORD];
    // Launch named temps from csv file
    get_namedtemps_file_name(ac_l_folder_lang);
    csv_parse(ac_l_folder_lang, s_named_temps.named_temps_top_of_page_line,
                i_named_temps_handler, &s_l_nt_parsed);

    // Change the size of items to match what was read from file
    s_named_temps_page.items.size = s_l_nt_parsed.i_nb_values;
}

/**
 * @brief Save the persist parameters.
 *
 * @param ps_c_menu
 */
static void named_temp_save(menu_t *ps_c_menu) {
    int i_l_persist_changed = 0;
    debug_log("curline: %d, top_of_page_line: %d",
              s_named_temps_page.current_line,
              s_named_temps.named_temps_top_of_page_line);
    if (persist.named_temps_top_of_page_line != s_named_temps.named_temps_top_of_page_line) {
        persist.named_temps_top_of_page_line = s_named_temps.named_temps_top_of_page_line;
        i_l_persist_changed = 1;
    }
    if (persist.named_temps_cur_line != s_named_temps_page.current_line) {
        persist.named_temps_cur_line = s_named_temps_page.current_line;
        i_l_persist_changed = 1;
    }
    if (i_l_persist_changed) {
        enqueue_action(persist_write);
        debug_log("named_temp_save end");
    }
}

/**
 * @brief Upon page down, the next values shall be read from file.
 *
 * @param ps_c_menu ignored
 */
static void named_temps_pgdown(menu_t *ps_c_menu) {
    if (s_named_temps_page.items.size == MENU_HEIGHT) {
        char folder_lang[LP_MAX_WORD];
        get_namedtemps_file_name(folder_lang);
        // Page was completely filled -> maybe some more elements in file
        named_temps_parsing_t s_l_nt_parsed;
        s_l_nt_parsed.i_nb_values = 0;
        s_named_temps.named_temps_top_of_page_line += MENU_HEIGHT;
        // Launch named temps from csv file
        get_namedtemps_file_name(folder_lang);
        csv_parse(folder_lang,
                  s_named_temps.named_temps_top_of_page_line, i_named_temps_handler,
                  &s_l_nt_parsed);
        if (s_l_nt_parsed.i_nb_values > 0) {
            // Do something only if there were remaining items in csv file
            s_named_temps_page.items.size = s_l_nt_parsed.i_nb_values;
            s_named_temps_page.current_line = 0;
            s_named_temps_page.current_posn = 0;
        } else {
            // Nothing found, keep the same top_of_page_line
            s_named_temps.named_temps_top_of_page_line -= MENU_HEIGHT;
            // And put cursor at last display line
            s_named_temps_page.current_line = MENU_HEIGHT - 1;
            s_named_temps_page.current_posn = s_named_temps_page.current_line;
        }
    } else {
        // Did not change page, but user friendly to put cursor at the end of
        // the page
        s_named_temps_page.current_line = s_named_temps_page.items.size - 1;
        s_named_temps_page.current_posn = s_named_temps_page.current_line;
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
    if (s_named_temps.named_temps_top_of_page_line >= MENU_HEIGHT) {
        // We are not on first page, go back one page
        s_named_temps.named_temps_top_of_page_line -= MENU_HEIGHT;

        named_temps_parsing_t s_l_nt_parsed;
        char folder_lang[LP_MAX_WORD];
        get_namedtemps_file_name(folder_lang);
        s_l_nt_parsed.i_nb_values = 0;
        // Launch named temps from csv file
        csv_parse(folder_lang,
                  s_named_temps.named_temps_top_of_page_line, i_named_temps_handler,
                  &s_l_nt_parsed);

        if (s_l_nt_parsed.i_nb_values != 0) {
            // Change the size of items to match what was read from file
            s_named_temps_page.items.size = s_l_nt_parsed.i_nb_values;
            return 1;
        } else {
            // if no other item found, re-set the top of page line
            s_named_temps.named_temps_top_of_page_line += MENU_HEIGHT;
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
 * @param ps_c_menu ignored
 */
static void named_temps_pgup(menu_t *ps_c_menu) {
    if (s_named_temps_page.current_line == 0) {
        named_temps_load_pgup();
    }
    s_named_temps_page.current_line = 0;
    s_named_temps_page.current_posn = 0;
    menu_event_display();
}

/**
 * @brief Up key.
 *
 * @param ps_c_menu unused
 */
static void named_temps_up(menu_t *ps_c_menu) {
    if (s_named_temps_page.current_line != 0) {
        // We are not on first line, so just standard move up
        menupage_up(ps_c_menu);
    } else {
        // Will change page only if we are not in the first "page" of the csv
        // file
        if (named_temps_load_pgup()) {
            // Items were loaded, place cursor at the bottom of the page
            s_named_temps_page.current_line = MENU_HEIGHT - 1;
            s_named_temps_page.current_posn = MENU_HEIGHT - 1;
            menu_event_display();
        }
    }
}

static void named_temps_down(menu_t *ps_c_menu) {
    if (s_named_temps_page.items.size == MENU_HEIGHT) {
        // Page is full
        if (s_named_temps_page.current_line < MENU_HEIGHT - 1) {
            // Down only if not at last screen line
            menupage_down(ps_c_menu);
        } else {
            // As page is full, maybe some more elements in next page
            named_temps_pgdown(ps_c_menu);
        }
    } else if (s_named_temps_page.items.size >
               s_named_temps_page.current_line + 1) {
        // Page is partially filled, down only if not at last element, and don't
        // page down (we were already at end of file).
        menupage_down(ps_c_menu);
    }
}

static void apply_named_temp(const menuitem_t *item) {
    send_to_intercom(IC_SET_WB, WB_MODE_COLORTEMP);
    send_to_intercom(IC_SET_COLOR_TEMP, *item->parm.menuitem_int.value);
    beep();
    menu_close();
}


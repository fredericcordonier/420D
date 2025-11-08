#include "firmware/gui.h"

/**
 * @file menu_main.c
 * @brief Management of the main menu.
 *
 */

/**
 * @defgroup menu The Menu
 *
 *  <H1>Accessing The Menu</H1>
 *
 * To access 400plus' menu, press the DP (DIRECT PRINT) button from within the
 * main camera dialog; to exit 400plus' menu, just half-press the
 * shutter-release button. The DP button is located on the rear of the camera,
 * at the left of the viewfinder, and beside the blue printer icon with a
 * squiggly arrow line next to it.
 *
 * <H1>Navigating The Menu</H1>
 *
 * 400plus features a multi-page menu, where each page is dedicated to a certain
 * task. To navigate the pages, use the front wheel (notice the '<<' and '>>'
 * indicators at the header of the menu). There is also a method to quickly
 * access any page: press and hold the AV button down, then a list of all
 * available pages will appear; now use the front dial to select a page, and
 * release the AV button to jump into it. Moreover, if the "Navigate to main"
 * option is active (see Settings Page), you can also press and immediately
 * release the AV button to stay in the main page; once there, just use UP (ISO)
 * and DOWN (WB) to select a page, and SET to enter it. To navigate within a
 * page, use the UP and DOWN buttons, (or ZOOM IN and ZOOM OUT, to move faster);
 *  then use the LEFT (METERING) and RIGHT (AF) buttons to change the value of
 * the selected item (hold these buttons down to change the value faster). Items
 * with an exclamation mark '!' at the left perform an action: activate it with
 * the SET button. And Items with a right sign '>' are sub-menus: enter them
 * with the RIGHT button, and return back to the main page with the AV button.
 * Notice that some items can both perform an action and have a sub-menu.
 *
 * <H1>Menu pages</H1>
 * <H2>Parameters Page</H2>
 *
 *  This page contains extensions to the camera's shooting parameters, and also
 * provides a quick access to some commonly-used, but hard to reach, custom
 * functions.
 *
 * | Item                |  Description                                |
 * |---------------------|---------------------------------------------|
 * | Av comp             |  Extended Exposure Compensation             |
 * | AEB                 |  Extended Exposure Compensation             |
 * | Color Temp. (K)     |  Custom White Balance                       |
 * | AutoISO             | @ref autoiso                                |
 * | ISO                 |  Extended ISOs                              |
 * | Safety Shift        |  Safety Shift                               |
 * | Mirror Lockup       | @ref mlu                                    |
 * | IR remote enable    |  IR Settings                                |
 * | Flash > Disable flash |  Flash Configuration                      |
 * | Flash > AF Flash    |  Other Shortcuts                            |
 * | Flash > Flash 2curt |  Other Shortcuts                            |
 *
 *  <H2>Scripts Page</H2>
 * Scripts are little programs that control your camera, and automate certain
 * tasks. | Item            | Description              |
 * |-----------------|--------------------------|
 * | Extended AEB    | The Extended AEB Script  |
 * | Flash AEB       | The Flash AEB Script     |
 * | Aperture AEB    | The Aperture AEB Script  |
 * | ISO AEB         | The ISO AEB Script       |
 * | Intervalometer  | The Intervalometer       |
 * | Bulb ramping    | Bulb Ramping             |
 * | Hand Waving     | The Hand Waving Script   |
 * | Self Timer      | The Self-Timer           |
 * | Long Exposure   | The Long Exposure Script |
 * | DOF Calculator  | The DOF Calculator       |
 *
 * <H2>Info Page</H2>
 * This page contains some useful information about your camera and 400plus.
 * | Item          | Description |
 * |---------------|------------------------------------------------------------------------------|
 * | Version       | The version number of the 400plus software currently
 * installed.              | | Release count | The total number of shots
 * (actuations) made by this camera body.             | | Body ID       | The
 * "BodyID" of the camera body (an internal number, unique to each camera). | |
 * Firmware      | The firmware version (should be 1.1.1 for 400plus to be
 * working).            | | Owner         | The owner of this camera (use Canon
 * software to change it).                  |
 *
 * <H2>Settings Page</H2>
 * Some advanced settings about your camera and 400plus; please, see
 * Configuration, page 48 for more information. | Item                       |
 * Description                         |
 * |----------------------------|-------------------------------------|
 * | Internationalization       | Setting the language of 400plus     |
 * | Configuring Extended ISOs  | Configuring intermediate ISO values |
 * | Persisting AEB             | Keep AEB settings when powering off |
 * | Restore Configuration      | Restore configuration               |
 * | Configuring Scripts        | Configuring the scripts             |
 * | Configuring Buttons        | Configuring the buttons             |
 * | Configuring Custom Modes   | Configuring the custom modes        |
 * | Configuring Quick Exposure | Configuring the quick exposure      |
 * | Configuring Menus          | Configuring the menus               |
 * | Configuring Pages          | Configuring 400plus menu pages      |
 * | Developers Page            | Display developers menu             |
 *
 * <H1>Personalizing The Menu</H1>
 * While in any page within the menu, you have the ability to change the order
 * of the items to your own personal preference.
 *
 * To do this, just press the PLAY button to 'grab' an item (a small '>' will be
 * displayed at the left of the item).
 *
 * Now use the UP and DOWN buttons to position the item where you would like it
 * to reside (you will see the item actually move to the new position). Finish
 * the process by pressing the PLAY button again, to save your changes.
 */

#include "firmware.h"
#include "macros.h"
#include "main.h"

#include "cmodes.h"
#include "languages.h"
#include "menu.h"
#include "menu_cmodes.h"
#include "menu_developer.h"
#include "menu_info.h"
#include "menu_params.h"
#include "menu_scripts.h"
#include "menu_settings.h"
#include "menuitem.h"
#include "menupage.h"
#include "persist.h"
#include "settings.h"
#include "snapshots.h"
#include "utils.h"

#include "menu_main.h"

int changed;

static void list_up(menu_t *menu);
static void list_down(menu_t *menu);

static void list_display(menu_t *menu);

static void list_hide(menu_t *menu);

static void page_display(menu_t *menu);

/**
 * @brief List of main pages.
 *
 */
menupage_t *menu_main_pages[] = {
    [MENUPAGE_PARAMS] = &menupage_params,
    [MENUPAGE_SCRIPTS] = &menupage_scripts,
    [MENUPAGE_INFO] = &menupage_info,
    [MENUPAGE_SETTINGS] = &menupage_settings,
    [MENUPAGE_CMODES] = &menupage_cmodes,
};

/**
 * @brief Definition of main menu and its events.
 *
 */
menu_t menu_main = {
    pages : LIST(menu_main_pages),
    ordering : menu_order.main,
    actions : {
        [MENU_EVENT_PLAY] = menupage_drag_drop,
        [MENU_EVENT_UP] = menupage_up,
        [MENU_EVENT_DOWN] = menupage_down,
        [MENU_EVENT_SET] = menu_set,
        [MENU_EVENT_LEFT] = menu_left,
        [MENU_EVENT_RIGHT] = menu_right,
        [MENU_EVENT_NEXT] = menu_next,
        [MENU_EVENT_PREV] = menu_prev,
        [MENU_EVENT_IN] = menupage_pgdown,
        [MENU_EVENT_OUT] = menupage_pgup,
        [MENU_EVENT_DISPLAY] = menupage_display,
        [MENU_EVENT_REFRESH] = menupage_refresh,
        [MENU_EVENT_FINISH] = menu_finish,
        [MENU_EVENT_SAVE] = menu_main_save,
        [MENU_EVENT_AV] = list_display,
        [MENU_EVENT_TRASH] = menupage_developer_start,
    }
};

/**
 * @brief Definition of main menu items.
 *
 */
menuitem_t main_list_items[] = {
    [MENUPAGE_PARAMS] = MENUITEM_PAGE(0, LP_WORD(L_P_PARAMS)),
    [MENUPAGE_SCRIPTS] = MENUITEM_PAGE(0, LP_WORD(L_P_SCRIPTS)),
    [MENUPAGE_INFO] = MENUITEM_PAGE(0, LP_WORD(L_P_INFO)),
    [MENUPAGE_SETTINGS] = MENUITEM_PAGE(0, LP_WORD(L_P_SETTINGS)),
    [MENUPAGE_CMODES] = MENUITEM_PAGE(0, LP_WORD(L_P_CMODES)),
};

/**
 * @brief Definition of main menu page.
 *
 */
menupage_t main_list = {
    name : LP_WORD(L_P_420D),
    items : LIST(main_list_items),
    actions : {
        [MENU_EVENT_PLAY] = page_display,
        [MENU_EVENT_UP] = list_up,
        [MENU_EVENT_DOWN] = list_down,
        [MENU_EVENT_PREV] = list_up,
        [MENU_EVENT_NEXT] = list_down,
        [MENU_EVENT_AV_UP] = list_hide,
        [MENU_EVENT_SET] = page_display,
    },
    ordering : menu_order.main,
};

/**
 * @brief Start the main menu.
 *
 */
void menu_main_start() {
    if (settings.menu_autosave)
        menu_main.current_posn = persist.last_page;

    menu_create(&menu_main);

    if (settings.menu_entermain)
        list_display(&menu_main);
}

/**
 * @brief Save the menus order.
 *
 * @param menu
 */
void menu_main_save(menu_t *menu) {
    if (persist.last_page != menu_main.current_posn ||
        persist.aeb != DPData.ae_bkt) {
        persist.last_page = menu_main.current_posn;
        persist.aeb = DPData.ae_bkt;

        if (persist.aeb)
            persist.last_aeb = persist.aeb;

        enqueue_action(persist_write);
    }

    if (menu->changed) {
        enqueue_action(settings_write);
        enqueue_action(cmodes_write);
        enqueue_action(lang_pack_config);
    }
}

/**
 * @brief Display a list of menu entries.
 *
 * @param menu Pointer to menu containing items.
 */
void list_display(menu_t *menu) {
    main_list.current_line = 2;
    main_list.current_posn = menu->current_posn;

    menu_set_page(&main_list);
}

/**
 * @brief Go to the previous menu entry (up).
 *
 * @param menu The menu.
 */
static void list_up(menu_t *menu) {
    menupage_t *page = menu->current_page;

    page->current_posn--;

    menu_event_display();
}

/**
 * @brief Go to the next menu entry (down).
 *
 * @param menu The menu.
 */
static void list_down(menu_t *menu) {
    menupage_t *page = menu->current_page;

    page->current_posn++;

    menu_event_display();
}

/**
 * @brief ??
 *
 * @param menu  The menu.
 */
static void list_hide(menu_t *menu) {
    menupage_t *page = menu->current_page;

    if (page->current_posn != menu->current_posn || !settings.menu_navmain)
        menu_set_posn(get_item_id(page, page->current_posn));
}

/**
 * @brief Display the current page of a menu.
 *
 * @param menu The menu.
 */
static void page_display(menu_t *menu) {
    menupage_t *page = menu->current_page;

    menu_set_posn(get_item_id(page, page->current_posn));
}

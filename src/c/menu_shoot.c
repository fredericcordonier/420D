/**
 * @file menu_shoot.c
 * @author Frédéric Cordonier
 * @brief Menu for various dialog menus.
 * @version 0.1
 * @date 2024-04-07
 *
 * @copyright Copyright (c) 2024
 *
 * Menu Shooting mode: groups scripts related to timed shooting:
 * | Item            | Description              |
 * |-----------------|--------------------------|
 * | Intervalometer  | The Intervalometer       |
 * | Hand Waving     | The Hand Waving Script   |
 * | Self Timer      | The Self-Timer           |
 * | Long Exposure   | The Long Exposure Script |
 *
 * This menu is displayed when pressing DP while in Drive selection dialog.
 *
 */

#include "firmware/gui.h"
#include <string.h>

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
#include "settings.h"
#include "utils.h"    // beep function

#include "menu_shoot.h"
#include "scripts.h"

typedef struct {
  int vformat;
  int rectime;
  int playtime;
} timelapse_calc;

typedef struct {
  int iso;
  int av;
  int tv;
  int ev;
} long_exp_calc;

static union {
  long_exp_calc long_exp;
  timelapse_calc timelapse;
} calc_parameters;

static void menu_lexp_calc_open(menu_t *menu);
static void menu_SHOOT_MODE_open_timelapse(menu_t *menu);
static void menu_SHOOT_MODE_update_timelapse(const menuitem_t *item);
static void menu_SHOOT_MODE_calc_timelapse(void);
static void menu_SHOOT_MODE_interval(const menuitem_t *item);
static void menu_SHOOT_MODE_wave(const menuitem_t *item);
static void menu_SHOOT_MODE_self_timer(const menuitem_t *item);
static void menu_SHOOT_MODE_long_exp(const menuitem_t *item);
static void menu_SHOOT_MODE_apply_calc_ev(const menuitem_t *item);
static void menu_SHOOT_MODE_apply_calc_tv(const menuitem_t *item);
static void menu_SHOOT_MODE_apply_calc(const menuitem_t *item);

static void menu_SHOOT_MODE_launch(action_t script);

static menuitem_t timer_items[] = {
    MENUITEM_TIMEOUT(0, LP_WORD(L_I_TIME), &settings.timer_timeout, NULL),
    MENUITEM_ACTION(0, LP_WORD(L_I_ACTION), &settings.timer_action, NULL)};

static menupage_t timer_page = {
  name : LP_WORD(L_S_TIMER),
  items : LIST(timer_items),
  actions : {
      [MENU_EVENT_AV] = menu_return,
  }
};

static menuitem_t wave_items[] = {
    MENUITEM_BOOLEAN(0, LP_WORD(L_I_DELAY), &settings.wave_delay, NULL),
    MENUITEM_ACTION(0, LP_WORD(L_I_ACTION), &settings.wave_action, NULL),
    MENUITEM_BOOLEAN(0, LP_WORD(L_I_REPEAT), &settings.wave_repeat, NULL),
    MENUITEM_BOOLEAN(0, LP_WORD(L_I_INSTANT), &settings.wave_instant, NULL)};

static menupage_t wave_page = {
  name : LP_WORD(L_S_HANDWAVE),
  items : LIST(wave_items),
  actions : {
      [MENU_EVENT_AV] = menu_return,
  }
};

static menuitem_t lexp_calc_items[] = {
    MENUITEM_BASEISO(0, LP_WORD(L_I_ISO), &(calc_parameters.long_exp.iso),
                     menu_SHOOT_MODE_apply_calc_ev),
    MENUITEM_AV(0, LP_WORD(L_I_AV_VAL), &(calc_parameters.long_exp.av),
                menu_SHOOT_MODE_apply_calc_ev),
    MENUITEM_TIMEOUT(0, LP_WORD(L_I_TV_VAL), &(calc_parameters.long_exp.tv),
                     menu_SHOOT_MODE_apply_calc_ev),
    MENUITEM_EVINFO(0, LP_WORD(L_I_EV_VAL), &(calc_parameters.long_exp.ev),
                    menu_SHOOT_MODE_apply_calc_tv),
    MENUITEM_LAUNCH(0, LP_WORD(L_I_APPLY), menu_SHOOT_MODE_apply_calc),
};

static menupage_t lexp_calc_page = {
  name : LP_WORD(L_S_CALCULATOR),
  items : LIST(lexp_calc_items),
  actions : {
      [MENU_EVENT_OPEN] = menu_lexp_calc_open,
      [MENU_EVENT_AV] = menu_return,
  }
};

static menuitem_t lexp_items[] = {
    MENUITEM_BOOLEAN(0, LP_WORD(L_I_DELAY), &settings.lexp_delay, NULL),
    MENUITEM_TIMEOUT(0, LP_WORD(L_I_EXPOSURE), &settings.lexp_time, NULL),
    MENUITEM_SUBMENU(0, LP_WORD(L_S_CALCULATOR), &lexp_calc_page, NULL),
};

static menupage_t lexp_page = {
  name : LP_WORD(L_S_LEXP),
  items : LIST(lexp_items),
  actions : {
      [MENU_EVENT_AV] = menu_return,
  }
};

static menuitem_t interval_items[] = {
    MENUITEM_BOOLEAN(0, LP_WORD(L_I_DELAY), &settings.interval_delay, NULL),
    MENUITEM_ACTION(0, LP_WORD(L_I_ACTION), &settings.interval_action, NULL),
    MENUITEM_TIMEOUT(0, LP_WORD(L_I_INTERVAL), &settings.interval_time,
                     menu_SHOOT_MODE_update_timelapse),
    MENUITEM_COUNTER(0, LP_WORD(L_I_SHOTS), &settings.interval_shots,
                     menu_SHOOT_MODE_update_timelapse),
    MENUITEM_VFORMAT(0, LP_WORD(L_I_VFORMAT),
                     &(calc_parameters.timelapse.vformat),
                     menu_SHOOT_MODE_update_timelapse),
    MENUITEM_INFTIME(0, LP_WORD(L_I_RECTIME),
                     &(calc_parameters.timelapse.rectime)),
    MENUITEM_INFTIME(0, LP_WORD(L_I_PLAYTIME),
                     &(calc_parameters.timelapse.playtime)),
};

static menupage_t interval_page = {
  name : LP_WORD(L_S_INTERVAL),
  items : LIST(interval_items),
  actions : {
      [MENU_EVENT_OPEN] = menu_SHOOT_MODE_open_timelapse,
      [MENU_EVENT_AV] = menu_return,
  }
};

static menuitem_t menupage_shoot_modes_items[] = {
    MENUITEM_SUBMENU(MENUPAGE_SHOOT_MODE_TIMER, LP_WORD(L_S_TIMER), &timer_page,
                     menu_SHOOT_MODE_self_timer),
    MENUITEM_SUBMENU(MENUPAGE_SHOOT_MODE_HANDWAVE, LP_WORD(L_S_HANDWAVE),
                     &wave_page, menu_SHOOT_MODE_wave),
    MENUITEM_SUBMENU(MENUPAGE_SHOOT_MODE_LEXP, LP_WORD(L_S_LEXP), &lexp_page,
                     menu_SHOOT_MODE_long_exp),
    MENUITEM_SUBMENU(MENUPAGE_SHOOT_MODE_INTERVAl, LP_WORD(L_S_INTERVAL),
                     &interval_page, menu_SHOOT_MODE_interval),
};

static menupage_t menupage_shoot_mode = {
  name : LP_WORD(L_P_SHOOTMODE),
  items : LIST(menupage_shoot_modes_items),
};

static menupage_t *menu_dialog_pages[] = {
    &menupage_shoot_mode,
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
    }
};

/**
 * Start the shooting modes dialog (intervallometer, wave,
 * timer...).
 *
 */
void menu_shoot_mode_start(void) {
  menupage_shoot_mode.current_line = 0;
  menupage_shoot_mode.current_posn = 0;
  menu_dialogs.color = MENU_COLOR_YELLOW;
  menu_dialogs.current_posn = 0;
  menu_create(&menu_dialogs);
}

static void menu_lexp_calc_open(menu_t *menu) {
  // Copy current parameters from camera to menu
  calc_parameters.long_exp.iso = DPData.iso;
  calc_parameters.long_exp.av = DPData.av_val;

  // Start with a common exposure time
  calc_parameters.long_exp.tv = settings.lexp_time;

  // Adjust exposure compensation
  calc_parameters.long_exp.ev =
      ec_normalize(8.0f * (float_log2(calc_parameters.long_exp.tv) +
                           (DPData.tv_val - TV_SEC) / 8.0f));
}

static void menu_SHOOT_MODE_open_timelapse(menu_t *menu) {
  calc_parameters.timelapse.vformat = VIDEO_FORMAT_25FPS;
  menu_SHOOT_MODE_calc_timelapse();
}

static void menu_SHOOT_MODE_update_timelapse(const menuitem_t *item) {
  menu_SHOOT_MODE_calc_timelapse();
  menu_event_display();
}

static void menu_SHOOT_MODE_calc_timelapse(void) {
  calc_parameters.timelapse.rectime =
      settings.interval_shots * settings.interval_time;

  switch (calc_parameters.timelapse.vformat) {
  case VIDEO_FORMAT_25FPS:
    calc_parameters.timelapse.playtime = settings.interval_shots / 25;
    break;
  case VIDEO_FORMAT_30FPS:
    calc_parameters.timelapse.playtime = settings.interval_shots / 30;
    break;
  case VIDEO_FORMAT_50FPS:
    calc_parameters.timelapse.playtime = settings.interval_shots / 50;
    break;
  case VIDEO_FORMAT_60FPS:
    calc_parameters.timelapse.playtime = settings.interval_shots / 60;
    break;
  default:
    calc_parameters.timelapse.playtime = 0;
    break;
  }
}

static void menu_SHOOT_MODE_interval(const menuitem_t *item) {
  menu_SHOOT_MODE_launch(script_interval);
}

static void menu_SHOOT_MODE_wave(const menuitem_t *item) {
  menu_SHOOT_MODE_launch(script_wave);
}

static void menu_SHOOT_MODE_self_timer(const menuitem_t *item) {
  menu_SHOOT_MODE_launch(script_self_timer);
}

static void menu_SHOOT_MODE_long_exp(const menuitem_t *item) {
  menu_SHOOT_MODE_launch(script_long_exp);
}

static void menu_SHOOT_MODE_launch(action_t script) {
  enqueue_action(menu_close);
  enqueue_action(script);
}

static void menu_SHOOT_MODE_apply_calc_ev(const menuitem_t *item) {
  float camera_ev = (DPData.iso - ISO_100) / 8.0f -
                    (DPData.av_val - AV_MIN) / 8.0f -
                    (DPData.tv_val - TV_SEC) / 8.0f;
  float script_ev = (calc_parameters.long_exp.iso - ISO_100) / 8.0f -
                    (calc_parameters.long_exp.av - AV_MIN) / 8.0f +
                    float_log2(calc_parameters.long_exp.tv);

  int ev = 8.0f * (script_ev - camera_ev);

  ev = CLAMP(ev, EC_MIN_EXT, EC_MAX_EXT);

  calc_parameters.long_exp.ev = ec_normalize(ev);
  menu_event_display();
}

static void menu_SHOOT_MODE_apply_calc_tv(const menuitem_t *item) {}

static void menu_SHOOT_MODE_apply_calc(const menuitem_t *item) {
  settings.lexp_time = calc_parameters.long_exp.tv;

  // send_to_intercom(IC_SET_AV_VAL, calc_parameters.long_exp.av);
  // send_to_intercom(IC_SET_ISO,    calc_parameters.long_exp.iso);

  calc_parameters.long_exp.ev = EV_ZERO;
  menu_return(NULL); // TODO:FixMe
}

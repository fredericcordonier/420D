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
  int i_vformat;
  int i_rectime;
  int i_playtime;
} timelapse_calc_t;

typedef struct {
  int i_iso;
  int i_av;
  int i_tv;
  int i_ev;
} long_exp_calc_t;

static union {
  long_exp_calc_t s_long_exp;
  timelapse_calc_t s_timelapse;
} u_calc_parameters;

static void menu_lexp_calc_open(menu_t *ps_x_menu);
static void menu_shoot_mode_open_timelapse(menu_t *ps_x_menu);
static void menu_shoot_mode_open_intervalometer(menu_t *ps_x_menu);
static void menu_shoot_mode_update_timelapse(const menuitem_t *ps_c_item);
static void menu_shoot_mode_calc_timelapse(void);
static void menu_shoot_mode_interval(const menuitem_t *ps_c_item);
static void menu_shoot_mode_timelapse(const menuitem_t *ps_c_item);
static void menu_shoot_mode_wave(const menuitem_t *ps_c_item);
static void menu_shoot_mode_self_timer(const menuitem_t *ps_c_item);
static void menu_shoot_mode_long_exp(const menuitem_t *ps_c_item);
static void menu_shoot_mode_apply_calc_ev(const menuitem_t *ps_c_item);
static void menu_shoot_mode_apply_calc_tv(const menuitem_t *ps_c_item);
static void menu_shoot_mode_apply_calc(const menuitem_t *ps_c_item);

static void menu_shoot_mode_launch(action_t s_x_script);

static menuitem_t a_timer_items[] = {
    MENUITEM_TIMEOUT(0, LP_WORD(L_I_TIME), &settings.timer_timeout, NULL),
    MENUITEM_ACTION(0, LP_WORD(L_I_ACTION), &settings.timer_action, NULL)};

static menupage_t s_timer_page = {
  name : LP_WORD(L_S_TIMER),
  items : LIST(a_timer_items),
  actions : {
      [MENU_EVENT_AV] = menu_return,
  }
};

static menuitem_t as_wave_items[] = {
    MENUITEM_BOOLEAN(0, LP_WORD(L_I_DELAY), &settings.wave_delay, NULL),
    MENUITEM_ACTION(0, LP_WORD(L_I_ACTION), &settings.wave_action, NULL),
    MENUITEM_BOOLEAN(0, LP_WORD(L_I_REPEAT), &settings.wave_repeat, NULL),
    MENUITEM_BOOLEAN(0, LP_WORD(L_I_INSTANT), &settings.wave_instant, NULL)};

static menupage_t s_wave_page = {
  name : LP_WORD(L_S_HANDWAVE),
  items : LIST(as_wave_items),
  actions : {
      [MENU_EVENT_AV] = menu_return,
  }
};

static menuitem_t as_lexp_calc_items[] = {
    MENUITEM_BASEISO(0, LP_WORD(L_I_ISO), &(u_calc_parameters.s_long_exp.i_iso),
                     menu_shoot_mode_apply_calc_ev),
    MENUITEM_AV(0, LP_WORD(L_I_AV_VAL), &(u_calc_parameters.s_long_exp.i_av),
                menu_shoot_mode_apply_calc_ev),
    MENUITEM_TIMEOUT(0, LP_WORD(L_I_TV_VAL), &(u_calc_parameters.s_long_exp.i_tv),
                     menu_shoot_mode_apply_calc_ev),
    MENUITEM_EVINFO(0, LP_WORD(L_I_EV_VAL), &(u_calc_parameters.s_long_exp.i_ev),
                    menu_shoot_mode_apply_calc_tv),
    MENUITEM_LAUNCH(0, LP_WORD(L_I_APPLY), menu_shoot_mode_apply_calc),
};

static menupage_t s_lexp_calc_page = {
  name : LP_WORD(L_S_CALCULATOR),
  items : LIST(as_lexp_calc_items),
  actions : {
      [MENU_EVENT_OPEN] = menu_lexp_calc_open,
      [MENU_EVENT_AV] = menu_return,
  }
};

static menuitem_t as_lexp_items[] = {
    MENUITEM_BOOLEAN(0, LP_WORD(L_I_DELAY), &settings.lexp_delay, NULL),
    MENUITEM_TIMEOUT(0, LP_WORD(L_I_EXPOSURE), &settings.lexp_time, NULL),
    MENUITEM_SUBMENU(0, LP_WORD(L_S_CALCULATOR), &s_lexp_calc_page, NULL),
};

static menupage_t s_lexp_page = {
  name : LP_WORD(L_S_LEXP),
  items : LIST(as_lexp_items),
  actions : {
      [MENU_EVENT_AV] = menu_return,
  }
};

static menuitem_t as_interval_items[] = {
    MENUITEM_BOOLEAN(0, LP_WORD(L_I_DELAY), &settings.interval_delay, NULL),
    MENUITEM_ACTION(0, LP_WORD(L_I_ACTION), &settings.interval_action, NULL),
    MENUITEM_TIMEOUT(0, LP_WORD(L_I_INTERVAL), &settings.interval_time,
                     menu_shoot_mode_update_timelapse),
    MENUITEM_COUNTER(0, LP_WORD(L_I_SHOTS), &settings.interval_shots,
                     menu_shoot_mode_update_timelapse),
    MENUITEM_VFORMAT(0, LP_WORD(L_I_VFORMAT),
                     &(u_calc_parameters.s_timelapse.i_vformat),
                     menu_shoot_mode_update_timelapse),
    MENUITEM_INFTIME(0, LP_WORD(L_I_RECTIME), &(u_calc_parameters.s_timelapse.i_rectime),
                    TRUE),
    MENUITEM_INFTIME(0, LP_WORD(L_I_PLAYTIME), &(u_calc_parameters.s_timelapse.i_playtime),
                    TRUE),
};

static menupage_t s_interval_page = {
  name : LP_WORD(L_S_INTERVAL),
  items : LIST(as_interval_items),
  actions : {
      [MENU_EVENT_OPEN] = menu_shoot_mode_open_intervalometer,
      [MENU_EVENT_AV] = menu_return,
  }
};

static menuitem_t as_timelapse_items[] = {
    MENUITEM_START_SCRIPT(0, LP_WORD(L_I_START_ACTION), &settings.timelapse_start, NULL),
    MENUITEM_VFORMAT(0, LP_WORD(L_I_VFORMAT), &settings.timelapse_vf,
                     NULL),
    MENUITEM_INFTIME(0, LP_WORD(L_I_RECTIME), &settings.timelapse_rectime,
                    FALSE),
    MENUITEM_INFTIME(0, LP_WORD(L_I_PLAYTIME), &settings.timelapse_playtime,
                    FALSE),
};

static menupage_t s_timelapse_page = {
  .name = LP_WORD(L_S_TIMELAPSE),
  .items = LIST(as_timelapse_items),
  .actions = {
      [MENU_EVENT_OPEN] = menu_shoot_mode_open_timelapse,
      [MENU_EVENT_AV] = menu_return,
  }
};

static menuitem_t menupage_shoot_modes_items[] = {
    MENUITEM_SUBMENU(MENUPAGE_SHOOT_MODE_TIMER, LP_WORD(L_S_TIMER), &s_timer_page,
                     menu_shoot_mode_self_timer),
    MENUITEM_SUBMENU(MENUPAGE_SHOOT_MODE_HANDWAVE, LP_WORD(L_S_HANDWAVE),
                     &s_wave_page, menu_shoot_mode_wave),
    MENUITEM_SUBMENU(MENUPAGE_SHOOT_MODE_LEXP, LP_WORD(L_S_LEXP), &s_lexp_page,
                     menu_shoot_mode_long_exp),
    MENUITEM_SUBMENU(MENUPAGE_SHOOT_MODE_INTERVAl, LP_WORD(L_S_INTERVAL),
                     &s_interval_page, menu_shoot_mode_interval),
    MENUITEM_SUBMENU(MENUPAGE_SHOOT_MODE_TIMELAPSE, LP_WORD(L_S_TIMELAPSE),
                    &s_timelapse_page, menu_shoot_mode_timelapse),
};

static menupage_t s_menupage_shoot_mode = {
  .name = LP_WORD(L_P_SHOOTMODE),
  .items = LIST(menupage_shoot_modes_items),
};

static menupage_t *as_menu_shoot_mode_pages[] = {
    &s_menupage_shoot_mode,
};

static menu_t s_menu_shoot_mode = {
    pages : LIST(as_menu_shoot_mode_pages),
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
  s_menupage_shoot_mode.current_line = 0;
  s_menupage_shoot_mode.current_posn = 0;
  s_menu_shoot_mode.color = MENU_COLOR_YELLOW;
  s_menu_shoot_mode.current_posn = 0;
  menu_create(&s_menu_shoot_mode);
}

static void menu_lexp_calc_open(menu_t *ps_x_menu) {
  // Copy current parameters from camera to menu
  u_calc_parameters.s_long_exp.i_iso = DPData.iso;
  u_calc_parameters.s_long_exp.i_av = DPData.av_val;

  // Start with a common exposure time
  u_calc_parameters.s_long_exp.i_tv = settings.lexp_time;

  // Adjust exposure compensation
  u_calc_parameters.s_long_exp.i_ev =
      ec_normalize(8.0f * (float_log2(u_calc_parameters.s_long_exp.i_tv) +
                           (DPData.tv_val - TV_SEC) / 8.0f));
}

static void menu_shoot_mode_open_intervalometer(menu_t *ps_x_menu) {
  u_calc_parameters.s_timelapse.i_vformat = VIDEO_FORMAT_25FPS;
  menu_shoot_mode_calc_timelapse();
}

static void menu_shoot_mode_open_timelapse(menu_t *ps_x_menu) {
  u_calc_parameters.s_timelapse.i_vformat = VIDEO_FORMAT_25FPS;
  menu_shoot_mode_calc_timelapse();
}

static void menu_shoot_mode_update_timelapse(const menuitem_t *ps_c_item) {
  menu_shoot_mode_calc_timelapse();
  menu_event_display();
}

static void menu_shoot_mode_calc_timelapse(void) {
  u_calc_parameters.s_timelapse.i_rectime =
      settings.interval_shots * settings.interval_time;

  switch (u_calc_parameters.s_timelapse.i_vformat) {
  case VIDEO_FORMAT_25FPS:
    u_calc_parameters.s_timelapse.i_playtime = settings.interval_shots / 25;
    break;
  case VIDEO_FORMAT_30FPS:
    u_calc_parameters.s_timelapse.i_playtime = settings.interval_shots / 30;
    break;
  case VIDEO_FORMAT_50FPS:
    u_calc_parameters.s_timelapse.i_playtime = settings.interval_shots / 50;
    break;
  case VIDEO_FORMAT_60FPS:
    u_calc_parameters.s_timelapse.i_playtime = settings.interval_shots / 60;
    break;
  default:
    u_calc_parameters.s_timelapse.i_playtime = 0;
    break;
  }
}

static void menu_shoot_mode_interval(const menuitem_t *ps_c_item) {
  menu_shoot_mode_launch(script_interval);
}

static void menu_shoot_mode_timelapse(const menuitem_t *ps_c_item) {
  menu_shoot_mode_launch(script_timelapse);
}

static void menu_shoot_mode_wave(const menuitem_t *ps_c_item) {
  menu_shoot_mode_launch(script_wave);
}

static void menu_shoot_mode_self_timer(const menuitem_t *ps_c_item) {
  menu_shoot_mode_launch(script_self_timer);
}

static void menu_shoot_mode_long_exp(const menuitem_t *ps_c_item) {
  menu_shoot_mode_launch(script_long_exp);
}

static void menu_shoot_mode_launch(action_t script) {
  enqueue_action(menu_close);
  enqueue_action(script);
}

static void menu_shoot_mode_apply_calc_ev(const menuitem_t *ps_c_item) {
  float camera_ev = (DPData.iso - ISO_100) / 8.0f -
                    (DPData.av_val - AV_MIN) / 8.0f -
                    (DPData.tv_val - TV_SEC) / 8.0f;
  float script_ev = (u_calc_parameters.s_long_exp.i_iso - ISO_100) / 8.0f -
                    (u_calc_parameters.s_long_exp.i_av - AV_MIN) / 8.0f +
                    float_log2(u_calc_parameters.s_long_exp.i_tv);

  int ev = 8.0f * (script_ev - camera_ev);

  ev = CLAMP(ev, EC_MIN_EXT, EC_MAX_EXT);

  u_calc_parameters.s_long_exp.i_ev = ec_normalize(ev);
  menu_event_display();
}

static void menu_shoot_mode_apply_calc_tv(const menuitem_t *ps_c_item) {}

static void menu_shoot_mode_apply_calc(const menuitem_t *ps_c_item) {
  settings.lexp_time = u_calc_parameters.s_long_exp.i_tv;

  // send_to_intercom(IC_SET_AV_VAL, u_calc_parameters.s_long_exp.i_av);
  // send_to_intercom(IC_SET_ISO,    u_calc_parameters.s_long_exp.i_iso);

  u_calc_parameters.s_long_exp.i_ev = EV_ZERO;
  menu_return(NULL); // TODO:FixMe
}

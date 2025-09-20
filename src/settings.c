#include <ioLib.h>
#include <string.h>
#include <vxworks.h>

#include "firmware/fio.h"

#include "firmware.h"
#include "macros.h"
#include "main.h"

#include "exposure.h"
#include "utils.h"

#include "serialize.h"
#include "settings.h"

settings_t settings_default = {
    .use_dpad = TRUE,
    .autoiso_enable = FALSE,
    .autoiso_miniso = ISO_MIN,       // ISO100
    .autoiso_maxiso = ISO_MAX,       // ISO1600
    .autoiso_mintv = EV_CODE(13, 0), // 1/60s
    .autoiso_maxav = EV_CODE(1, 0),  // 1EV
    .autoiso_relaxed = FALSE,
    .eaeb_delay = FALSE,
    .eaeb_frames = 3,
    .eaeb_ev = EV_CODE(1, 0),               // 1EV
    .eaeb_tv_min = TV_BULB(EV_CODE(15, 0)), // 1/250s
    .eaeb_tv_max = TV_BULB(EV_CODE(13, 0)), // 1/60s
    .eaeb_direction = EAEB_DIRECTION_BOTH,
    .efl_aeb_delay = FALSE,
    .efl_aeb_frames = 3,
    .efl_aeb_ev = EV_CODE(1, 0), // 1EV
    .efl_aeb_direction = EAEB_DIRECTION_BOTH,
    .apt_aeb_delay = FALSE,
    .apt_aeb_frames = 3,
    .apt_aeb_ev = EV_CODE(1, 0), // 1EV
    .apt_aeb_direction = EAEB_DIRECTION_BOTH,
    .iso_aeb_delay = FALSE,
    .iso_aeb = {TRUE, TRUE, TRUE, TRUE, TRUE},
    .interval_delay = FALSE,
    .interval_time = 2,
    .interval_action = SHOT_ACTION_SHOT,
    .interval_shots = 0,
    .bramp_delay = FALSE,
    .bramp_time = 60,
    .bramp_shots = 100,
    .bramp_exp = 1,
    .bramp_ramp_t = 300,
    .bramp_ramp_s = 0,
    .bramp_ramp_exp = EV_CODE(1, 0),
    .bramp_ramp_time = EV_ZERO,
    .wave_delay = FALSE,
    .wave_action = SHOT_ACTION_SHOT,
    .wave_repeat = FALSE,
    .wave_instant = FALSE,
    .lexp_delay = FALSE,
    .lexp_time = 60,
    .remote_delay = FALSE,
    .timer_timeout = 5,
    .timer_action = SHOT_ACTION_SHOT,
    .keep_power_on = TRUE,
    .review_off = FALSE,
    .script_lcd = SCRIPT_LCD_KEEP,
    .script_indicator = SCRIPT_INDICATOR_MEDIUM,
    .debug_on_poweron = FALSE,
    .logfile_mode = 0,
    .remote_enable = FALSE,
    .developers_menu = FALSE,
    .shortcut_jump = SHORTCUT_ISO,
    .shortcut_trash = SHORTCUT_SCRIPT,
    .button_disp = FALSE,
    .language = 0,
    .digital_iso_step = 0,
    .menu_navmain = FALSE,
    .menu_entermain = FALSE,
    .menu_autosave = TRUE,
    .qexp_mintv = EV_CODE(13, 0), // 1/60s
    .qexp_weight = QEXP_WEIGHT_NONE,
    .persist_aeb = TRUE,
    .invert_olc = 0,
};

menu_order_t menu_order_default;

settings_t settings;
menu_order_t menu_order;

int settings_read() {
    int i;

    int result = FALSE;
    int file = -1;

    for (i = 0; i < LENGTH(menu_order_default.main); i++)
        menu_order_default.main[i] = i;

    for (i = 0; i < LENGTH(menu_order_default.params); i++)
        menu_order_default.params[i] = i;

    for (i = 0; i < LENGTH(menu_order_default.scripts); i++)
        menu_order_default.scripts[i] = i;

    for (i = 0; i < LENGTH(menu_order_default.info); i++)
        menu_order_default.info[i] = i;

    for (i = 0; i < LENGTH(menu_order_default.developer); i++)
        menu_order_default.developer[i] = i;

    for (i = 0; i < LENGTH(menu_order_default.settings); i++)
        menu_order_default.settings[i] = i;

    // settings_t    settings_buffer;
    // menu_order_t  menu_order_buffer;

    settings = settings_default;
    menu_order = menu_order_default;

    if ((file = FIO_OpenFile(MKPATH_NEW(SETTINGS_FILENAME), O_RDONLY)) != -1) {
        if (read_settings_file(file, &settings) != -1)
            result = TRUE;
        FIO_CloseFile(file);
    }
    if ((file = FIO_OpenFile(MKPATH_NEW(MENU_ORDER_FILENAME), O_RDONLY)) !=
        -1) {
        if (read_menu_order_file(file, &menu_order) != -1)
            result = TRUE;
        FIO_CloseFile(file);
    }
    return result;
}

void settings_write() {
    int file = -1;
    int success = 1;

    if ((file = FIO_OpenFile(MKPATH_NEW(SETTINGS_FILENAME),
                             O_CREAT | O_WRONLY)) != -1) {
        success = write_settings_file(file, &settings);
        // TODO: only settings are saved now, menu_order to do
        FIO_CloseFile(file);
    }
    if (success == -1) {
        // Don't want to have a partially written file here, delete it.
        FIO_RemoveFile(MKPATH_NEW(SETTINGS_FILENAME));
    }
    if ((file = FIO_OpenFile(MKPATH_NEW(MENU_ORDER_FILENAME),
                             O_CREAT | O_WRONLY)) != -1) {
        success = write_menu_order_file(file, &menu_order);
    }
    if (success == -1) {
        // Remove file partially written
        FIO_RemoveFile(MKPATH_NEW(MENU_ORDER_FILENAME));
    }
}

void settings_apply() {
    if (settings.remote_delay) {
        RemReleaseSelfMax = 4500;
        RemReleaseInstMin = 5560;
    } else {
        RemReleaseSelfMax = 6160;
        RemReleaseInstMin = 7410;
    }

    if (settings.remote_enable) {
        remote_on();
    } else {
        remote_off();
    }
}

void settings_restore() {
    settings = settings_default;
    menu_order = menu_order_default;

    settings_apply();
    settings_write();
}

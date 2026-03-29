#include <ioLib.h>
#include <string.h>
#include <vxworks.h>

#include "firmware/fio.h"

#include "debug.h"
#include "exposure.h"
#include "firmware.h"
#include "macros.h"
#include "main.h"
#include "utils.h"

#include "af_patterns.h"
#include "serialize.h"
#include "settings.h"

#define PARAM_INT_DEF(s, f, v)     .f = (v),
#define PARAM_INT_ARRAY_DEF(s, f, l, v)       .f = v,
#define ISO_AEB_DEFAULT   {TRUE, TRUE, TRUE, TRUE, TRUE}
settings_t settings_default = {
#include "../def/settings_t.def"
    // .iso_aeb = {TRUE, TRUE, TRUE, TRUE, TRUE},
};
#undef PARAM_INT_DEF
#undef PARAM_INT_ARRAY_DEF

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
        if (read_settings_file(file) != -1) {
            result = TRUE;
        }
        FIO_CloseFile(file);
    }
    return result;
}

void settings_write() {
    int file = -1;
    int success = 1;

    if ((file = FIO_OpenFile(MKPATH_NEW(SETTINGS_FILENAME),
                             O_CREAT | O_WRONLY)) != -1) {
        success = write_settings_file(file);
        // TODO: only settings are saved now, menu_order to do
        FIO_CloseFile(file);
    }
    if (success == -1) {
        // Don't want to have a partially written file here, delete it.
        FIO_RemoveFile(MKPATH_NEW(SETTINGS_FILENAME));
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

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

#include "fcache.h"

menu_order_t menu_order_default;

settings_t settings;
menu_order_t menu_order;

int settings_read() {
    int i;

    int result = FALSE;
    int file = -1;
    fcache s_l_file_read_cache;
    unsigned char buffer[1000]; // Buffer used for file read, to avoid dynamic memory

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

    settings = s_g_settings_t_default;
    menu_order = menu_order_default;

    file = fcache_open(&s_l_file_read_cache, MKPATH_NEW(SETTINGS_FILENAME), buffer, sizeof(buffer), O_RDONLY);
    if (file  != -1) {
        if (read_settings_file(&s_l_file_read_cache) != -1) {
            result = TRUE;
        }
        fcache_close_file(&s_l_file_read_cache);
    }
    return result;
}

void settings_write() {
    fcache s_l_file_write_cache;
    int file = -1;
    int success = 1;
    unsigned char buffer[1000]; // Buffer used for file write, to avoid dynamic memory allocation

    file = fcache_open(&s_l_file_write_cache, MKPATH_NEW(SETTINGS_FILENAME), buffer, sizeof(buffer), O_CREAT | O_WRONLY);
    if (file != -1) {
        success = write_settings_file(&s_l_file_write_cache);
        fcache_close_file(&s_l_file_write_cache);
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
    settings = s_g_settings_t_default;
    menu_order = menu_order_default;

    settings_apply();
    settings_write();
}

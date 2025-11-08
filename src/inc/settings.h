#ifndef SETTINGS_H_
#define SETTINGS_H_

#define SETTINGS_FILENAME "SETTINGS.INI"

#define SETTINGS_VERSION 0x40

#define CCT_COUNT 16

#include "languages.h"
#include "menu_developer.h"
#include "menu_info.h"
#include "menu_main.h"
#include "menu_params.h"
#include "menu_scripts.h"
#include "menu_settings.h"

typedef enum {
    SHOT_ACTION_SHOT,
    SHOT_ACTION_EXT_AEB,
    SHOT_ACTION_EFL_AEB,
    SHOT_ACTION_APT_AEB,
    SHOT_ACTION_ISO_AEB,
    SHOT_ACTION_LONG_EXP,
    SHOT_ACTION_COUNT,
    SHOT_ACTION_FIRST = 0,
    SHOT_ACTION_LAST = SHOT_ACTION_COUNT - 1
} shot_action_t;

typedef enum {
    SHORTCUT_NONE,
    SHORTCUT_ISO,
    SHORTCUT_SCRIPT,
    SHORTCUT_MLU,
    SHORTCUT_AEB,
    SHORTCUT_HACK_MENU,
    SHORTCUT_FLASH,
    SHORTCUT_DISPLAY,
    SHORTCUT_AFCFG,
#ifdef DEV_BTN_ACTION
    SHORTCUT_DEV_BTN,
#endif
    SHORTCUT_COUNT,
    SHORTCUT_FIRST = 0,
    SHORTCUT_LAST = SHORTCUT_COUNT - 1
} shortcut_t;

typedef enum {
    EAEB_DIRECTION_UP,
    EAEB_DIRECTION_DOWN,
    EAEB_DIRECTION_BOTH,
    EAEB_DIRECTION_COUNT,
    EAEB_DIRECTION_FIRST = 0,
    EAEB_DIRECTION_LAST = EAEB_DIRECTION_COUNT - 1
} eaeb_direction_t;

typedef enum {
    SCRIPT_INDICATOR_NONE,
    SCRIPT_INDICATOR_SLOW,
    SCRIPT_INDICATOR_MEDIUM,
    SCRIPT_INDICATOR_FAST,
    SCRIPT_INDICATOR_COUNT,
    SCRIPT_INDICATOR_FIRST = 0,
    SCRIPT_INDICATOR_LAST = SCRIPT_INDICATOR_COUNT - 1
} script_indicator_t;

typedef enum {
    SCRIPT_LCD_KEEP,
    SCRIPT_LCD_DIM,
    SCRIPT_LCD_OFF,
    SCRIPT_LCD_COUNT,
    SCRIPT_LCD_FIRST = 0,
    SCRIPT_LCD_LAST = SCRIPT_LCD_COUNT - 1
} script_lcd_t;

typedef enum {
    LOGFILE_MODE_OVERWRITE,
    LOGFILE_MODE_NEW,
    LOGFILE_MODE_APPEND,
    LOGFILE_MODE_COUNT,
    LOGFILE_MODE_FIRST = 0,
    LOGFILE_MODE_LAST = LOGFILE_MODE_COUNT - 1
} logfile_mode_t;

typedef enum {
    FLASH_MODE_ENABLED,
    FLASH_MODE_DISABLED,
    FLASH_MODE_EXTONLY,
    FLASH_MODE_COUNT,
    FLASH_MODE_FIRST = 0,
    FLASH_MODE_LAST = FLASH_MODE_COUNT - 1
} flash_mode_t;

typedef enum {
    QEXP_WEIGHT_NONE,
    QEXP_WEIGHT_AV,
    QEXP_WEIGHT_TV,
    QEXP_WEIGHT_COUNT,
    QEXP_WEIGHT_FIRST = 0,
    QEXP_WEIGHT_LAST = QEXP_WEIGHT_COUNT - 1
} qexp_weight_t;

#define PARAM_INT_DEF(s, f) int f;
#define PARAM_INT_ARRAY_DEF(s, f, i) int f[i];
typedef struct {
#include "settings.def"
} settings_t;

typedef struct {
#include "menu_order.def"
} menu_order_t;
#undef PARAM_INT_DEF
#undef PARAM_INT_ARRAY_DEF

extern settings_t settings;
extern menu_order_t menu_order;

int settings_read(void);
void settings_write(void);
void settings_apply(void);
void settings_restore(void);

#endif /* SETTINGS_H_ */

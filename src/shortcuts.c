/**
 * @file shortcuts.c
 * @brief Management of "shortcuts" (Trash/Jump buttons assigned to rapid
 * configurations).
 *
 */

/**
 * \defgroup disp_shortcut Better display
 *
 * \brief Management of "Better Display" feature.
 *
 * Better display feature allows to easily change the brightness of the display
 * (easier than going to the ad-hoc menu, 1st wrench).
 *
 * It is activated through the Settings menu, button config.
 *
 * If activated, pressing DISP will display the current brightness level in
 * place of ISO value. Pressing the D-Pad arrows change the value:
 * - Up: max level
 * - Down: min level
 * - Left: decrease brightness by one value
 * - Right: increase brightness by one value
 *
 */

/**
 * \defgroup flash_shortcut Rapid flash configuration
 *
 * \brief Management of the rapid flash configuration.
 *
 * Rapid flash configuration feature allows to set various flash parameters with
 * few button presses.
 *
 * It can be assigned to "Trash" or "Jump" button in the Settings menu, button
 * config.
 *
 * Flash configuration can be done by pressing and maintaining the assigned
 * button, and then pressing D-Pad:
 * - Set: activate/disactivate the flash
 * - Av: set 1st (ON) or 2nd curtain synchro
 * - Left: decrease Ev flash compensation by 1/2 step
 * - Right: increase Ev flash compensation by 1/2 step
 * - Up: decrease Ev flash compensation by 1 step
 * - Down: increase Ev flash compensation by 1 step
 * Flash exposure compensation can be defined from -6 to +6 Ev
 */

/**
 * \defgroup mlu_shortcut Rapid Mirror Lock-Up configuration
 *
 * \brief Management of the rapid Mirror Lock-Up configuration.
 *
 * Rapid Mirror Lock-Up configuration feature allows to set MLU parameters with
 * few button presses.
 *
 * It can be assigned to "Trash" or "Jump" button in the Settings menu, button
 * config.
 *
 * MLU configuration can be done by pressing and maintaining the assigned
 * button, and then pressing D-Pad:
 * - Set: activate/disactivate the MLU
 */
#include <string.h>

#include "firmware.h"

#include "macros.h"
#include "main.h"

#include "autoiso.h"
#include "display.h"
#include "intercom.h"
#include "persist.h"
#include "scripts.h"
#include "settings.h"
#include "utils.h"

#include "shortcuts.h"

const char *label_iso = SHORTCUT_LABEL_ISO;
const char *label_mlu = SHORTCUT_LABEL_MLU;
const char *label_aeb = SHORTCUT_LABEL_AEB;
const char *label_flash = SHORTCUT_LABEL_FLASH;
const char *label_display = SHORTCUT_LABEL_DISPLAY;

static void repeat_last_script(void);

static void shortcut_start(shortcut_t action);

static void shortcut_iso_toggle(void);
static void shortcut_aeb_toggle(void);

static void shortcut_iso_set(iso_t iso);
static void shortcut_mlu_toggle(void);
static void shortcut_efl_toggle(void);
static void shortcut_efl_set(ec_t value);
static void shortcut_f2c_toggle(void);
static void shortcut_aeb_set(ec_t value);
static void shortcut_disp_set(int brightness);

static void shortcut_info_iso(void);
static void shortcut_info_mlu(void);
static void shortcut_info_aeb(void);
static void shortcut_info_flash(void);
static void shortcut_info_display(void);

static void shortcut_info(const char *label);
static void shortcut_info_str(const char *label, const char *value);
static void shortcut_info_int(const char *label, const int value);
static void shortcut_info_ec(const char *label, const ec_t value);
static void shortcut_info_end(void);

/**
 * @brief Activate the JUMP shortcut.
 *
 */
void shortcut_jump() {
    if (DPData.cf_set_button_func != 4)
        shortcut_start(settings.shortcut_jump);
    /// This not available if D-Pad is configured for AF point selection
}

/**
 * @brief Activate the TRASH shortcut.
 *
 */
void shortcut_trash() {
    if (DPData.cf_set_button_func != 4)
        shortcut_start(settings.shortcut_trash);
    /// This not available if D-Pad is configured for AF point selection
}

/**
 * @brief Manage the DISP shortcut.
 * @ingroup disp_shortcut
 *
 * If "Better Display" is ON, pressing the DISP button will:
 * - if display is off, activate the display
 * - otherwise, SHORTCUT_DISPLAY is activated
 * If "Better Display" is OFF, post the DISP button to the original software.
 */
void shortcut_disp() {
    if (settings.button_disp) {
        if (FLAG_GUI_MODE == GUIMODE_OFF) {
            press_button(IC_BUTTON_DISP);
        } else {
            status.shortcut_running = SHORTCUT_DISPLAY;
            shortcut_info_display();
        }
    } else {
        press_button(IC_BUTTON_DISP);
    }
}

/**
 * @brief Start a shortcut.
 *
 * @param action Which shortcut to start.
 *
 */
static void shortcut_start(shortcut_t action) {
    char *msg = NULL;

    status.shortcut_running = action;

    switch (action) {
    case SHORTCUT_ISO:
        shortcut_info_iso();
        break;
    case SHORTCUT_SCRIPT:
        repeat_last_script();
        break;
    case SHORTCUT_MLU:
        shortcut_info_mlu();
        break;
    case SHORTCUT_AEB:
        shortcut_info_aeb();
        break;
    case SHORTCUT_HACK_MENU:
        menu_main_start();
        break;
    case SHORTCUT_FLASH:
        shortcut_info_flash();
        break;
    case SHORTCUT_DISPLAY:
        shortcut_info_display();
        break;
#ifdef DEV_BTN_ACTION
    case SHORTCUT_DEV_BTN:
        dev_btn_action();
        break;
#endif
    default:
        break;
    }

    if (msg) {
        dialog_item_set_str(hMainDialog, 0x26, msg);
        display_refresh();
    }
}

/**
 * @brief Stop the running shortcut.
 *
 */
void shortcut_stop() { status.shortcut_running = SHORTCUT_NONE; }

/**
 * @brief Beep if display is shut down during a "shortcut".
 * @ingroup disp_shortcut
 *
 */
void shortcut_event_disp() {
    press_button(IC_BUTTON_DISP);
    enqueue_action(beep);
    shortcut_event_end();
}

/**
 * @brief End the envent linked to shortcut.
 *
 */
void shortcut_event_end() {
    switch (status.shortcut_running) {
    case SHORTCUT_AEB:
        enqueue_action(persist_write);
        break;
    default:
        break;
    }

    status.shortcut_running = SHORTCUT_NONE;

    shortcut_info_end();
}

/**
 * @brief Pressing AV during a "shortcut" assigned to flash configuration will
 * toggle 2nc curtain flash.
 * @ingroup flash_shortcut
 *
 */
void shortcut_event_av(void) {
    switch (status.shortcut_running) {
    case SHORTCUT_FLASH:
        shortcut_f2c_toggle();
        break;
    default:
        break;
    }
}

/**
 * @brief Management of SET button during shortcut mode.
 *
 */
void shortcut_event_set(void) {
    switch (status.shortcut_running) {
    case SHORTCUT_ISO:
        shortcut_iso_toggle();
        break;
    case SHORTCUT_MLU:
        shortcut_mlu_toggle();
        break;
    case SHORTCUT_AEB:
        shortcut_aeb_toggle();
        break;
    case SHORTCUT_FLASH:
        shortcut_efl_toggle();
        break;
    case SHORTCUT_DISPLAY:
        enqueue_action(beep);
        shortcut_event_end();
        break;
    default:
        break;
    }
}

/**
 * @brief Management of UP button during shortcut mode.
 *
 */
void shortcut_event_up(void) {
    switch (status.shortcut_running) {
    case SHORTCUT_ISO:
        shortcut_iso_set(iso_next(DPData.iso));
        break;
    case SHORTCUT_AEB:
        shortcut_aeb_set(
            MIN((EV_TRUNC(DPData.ae_bkt) + EV_CODE(1, 0)), EC_MAX));
        break;
    case SHORTCUT_FLASH:
        shortcut_efl_set(
            MIN((EV_TRUNC(DPData.efcomp) + EV_CODE(1, 0)), EC_MAX));
        break;
    case SHORTCUT_DISPLAY:
        shortcut_disp_set(7);
        break;
    default:
        break;
    }
}

/**
 * @brief Management of DOWN button during shortcut mode.
 *
 */
void shortcut_event_down(void) {
    switch (status.shortcut_running) {
    case SHORTCUT_ISO:
        shortcut_iso_set(iso_prev(DPData.iso));
        break;
    case SHORTCUT_AEB:
        shortcut_aeb_set(
            MAX((EV_TRUNC(DPData.ae_bkt) - EV_CODE(1, 0)), EC_ZERO));
        break;
    case SHORTCUT_FLASH:
        shortcut_efl_set(
            MAX((EV_TRUNC(DPData.efcomp) - EV_CODE(1, 0)), EC_MIN));
        break;
    case SHORTCUT_DISPLAY:
        shortcut_disp_set(1);
        break;
    default:
        break;
    }
}

/**
 * @brief Management of RIGHT button during shortcut mode.
 *
 */
void shortcut_event_right(void) {
    switch (status.shortcut_running) {
    case SHORTCUT_ISO:
        shortcut_iso_set(iso_inc(DPData.iso));
        break;
    case SHORTCUT_AEB:
        shortcut_aeb_set(ec_inc(DPData.ae_bkt, FALSE));
        break;
    case SHORTCUT_FLASH:
        shortcut_efl_set(ec_inc(DPData.efcomp, FALSE));
        break;
    case SHORTCUT_DISPLAY:
        shortcut_disp_set(MIN(DPData.lcd_brightness + 1, 7));
        break;
    default:
        break;
    }
}

/**
 * @brief Management of LEFT button during shortcut mode.
 *
 */
void shortcut_event_left(void) {
    switch (status.shortcut_running) {
    case SHORTCUT_ISO:
        shortcut_iso_set(iso_dec(DPData.iso));
        break;
    case SHORTCUT_AEB:
        shortcut_aeb_set(MAX(ec_dec(DPData.ae_bkt, FALSE), EV_ZERO));
        break;
    case SHORTCUT_FLASH:
        shortcut_efl_set(ec_dec(DPData.efcomp, FALSE));
        break;
    case SHORTCUT_DISPLAY:
        shortcut_disp_set(MAX(DPData.lcd_brightness - 1, 1));
        break;
    default:
        break;
    }
}

/**
 * @brief Toggle Auto-ISO on-off.
 * @ingroup iso_shortcut
 *
 */
static void shortcut_iso_toggle() {
    settings.autoiso_enable = !settings.autoiso_enable;
    enqueue_action(settings_write);

    shortcut_info_iso();
}

/**
 * @brief Toggle AEB shortcut.
 * @ingroup aeb_shortcut
 *
 */
static void shortcut_aeb_toggle(void) {
    shortcut_aeb_set(DPData.ae_bkt ? EC_ZERO : persist.last_aeb);
}

/**
 * @brief Set ISO value and display it.
 * @ingroup iso_shortcut
 * @param iso ISO value.
 */
static void shortcut_iso_set(iso_t iso) {
    if (settings.autoiso_enable) {
        settings.autoiso_enable = FALSE;
        enqueue_action(settings_write);
        enqueue_action(beep);
    }

    send_to_intercom(IC_SET_ISO, iso);
    shortcut_info_iso();
}

/**
 * @brief Toggle the Mirror Lock-up (and display ON/OFF).
 * @ingroup mlu_shortcut
 */
static void shortcut_mlu_toggle(void) {
    send_to_intercom(IC_SET_CF_MIRROR_UP_LOCK, 1 - DPData.cf_mirror_up_lock);
    shortcut_info_mlu();
}

/**
 * @brief Toggle the flash ON/OFF and display this value.
 * @ingroup flash_shortcut
 */
static void shortcut_efl_toggle(void) {
    send_to_intercom(IC_SET_CF_EMIT_FLASH, 1 - DPData.cf_emit_flash);
    shortcut_info_flash();
}

/**
 * @brief Set the flash exposure compensation and display it.
 * @ingroup flash_shortcut
 * @param value Value of compensation to set.
 */
static void shortcut_efl_set(ec_t value) {
    send_to_intercom(IC_SET_EFCOMP, value);
    shortcut_info_flash();
}

/**
 * @brief Toggle 2nd Curtain activation and display the value.
 * @ingroup flash_shortcut
 */
static void shortcut_f2c_toggle(void) {
    if (DPData.cf_emit_flash)
        send_to_intercom(IC_SET_CF_FLASH_SYNC_REAR,
                         1 - DPData.cf_flash_sync_rear);

    shortcut_info_flash();
}

/**
 * @brief Set exposure compensation and display it.
 * @ingroup aeb_shortcut
 * @param value Value to be set and displayed.
 */
static void shortcut_aeb_set(ec_t value) {
    send_to_intercom(IC_SET_AE_BKT, value);

    persist.aeb = value;

    if (persist.aeb)
        persist.last_aeb = persist.aeb;

    shortcut_info_aeb();
}

/**
 * @brief Set brightness shortcut, display the value.
 * @ingroup disp_shortcut
 * @param brightness Value to be set and displayed.
 */
static void shortcut_disp_set(int brightness) {
    send_to_intercom(IC_SET_LCD_BRIGHTNESS, brightness);
    shortcut_info_display();
}

#ifdef DEV_BTN_ACTION
void dev_btn_action() {
    // quick shortcut for developers to test stuff
    beep();
    ptp_dump_info();
}
#endif

/**
 * @brief Repeat the last script.
 * @ingroup script_shortcut
 */
static void repeat_last_script(void) {
    switch (persist.last_script) {
    case SCRIPT_EXT_AEB:
        script_ext_aeb();
        break;
    case SCRIPT_EFL_AEB:
        script_efl_aeb();
        break;
    case SCRIPT_ISO_AEB:
        script_iso_aeb();
        break;
    case SCRIPT_INTERVAL:
        script_interval();
        break;
    case SCRIPT_WAVE:
        script_wave();
        break;
    case SCRIPT_TIMER:
        script_self_timer();
        break;
    default:
        break;
    }
}

/**
 * @brief Display ISO value.
 * @ingroup iso_shortcut
 */
static void shortcut_info_iso() {
    char buffer[8] = "AUTO";

    if (!settings.autoiso_enable)
        iso_print(buffer, DPData.iso);

    shortcut_info_str(label_iso, buffer);
}

/**
 * @brief Display MLU value
 * @ingroup mlu_shortcut
 *
 */
static void shortcut_info_mlu() {
    shortcut_info_str(label_mlu, DPData.cf_mirror_up_lock ? " On" : " Off");
}

/**
 * @brief Display AEB value.
 * @ingroup aeb_shortcut
 */
static void shortcut_info_aeb() { shortcut_info_ec(label_aeb, DPData.ae_bkt); }

/**
 * @brief Display flash on main display.
 * @ingroup flash_shortcut
 *
 */
static void shortcut_info_flash() {
    char buffer[8] = "";

    sprintf(buffer, "%s",
            DPData.cf_emit_flash ? (DPData.cf_flash_sync_rear ? "2nd" : "On")
                                 : "Off");
    shortcut_info_str(label_flash, buffer);
}

/**
 * @brief Display the LCD brightness level.
 * @ingroup disp_shortcut
 */
static void shortcut_info_display() {
    shortcut_info_int(label_display, DPData.lcd_brightness);
}

/**
 * @brief Display a text in the display area of "remaining card capacity"
 *
 * @param label
 */
static void shortcut_info(const char *label) {
    dialog_item_set_label(hMainDialog, 0x08, label, 1 + strlen(label), 0x26);

    display_refresh();
}

/**
 * @brief Display information about shortcuts on main display.
 *
 * @param label Shortcut function.
 * @param value Value of parameter.
 */
static void shortcut_info_str(const char *label, const char *value) {
    dialog_item_set_label(hMainDialog, 0x08, value, 1 + strlen(value), 0x04);

    shortcut_info(label);
}

static void shortcut_info_int(const char *label, const int value) {
    char buffer[8];

    sprintf(buffer, "%4i", value);
    shortcut_info_str(label, buffer);
}

/**
 * @brief Display exposure compensation value.
 *
 * @param label
 * @param value exposure compensation value
 */
static void shortcut_info_ec(const char *label, const ec_t value) {
    int symbol = get_efcomp_data(value);

    if (value != EC_ZERO)
        dialog_item_set_label(hMainDialog, 0x12, &symbol, 0x04, 0x09);

    shortcut_info(label);
}

/**
 * @brief Reset display info after a shortcut.
 *
 */
static void shortcut_info_end() {
    char label[8], value[8];
    const int symbol = 0xFC;

    sprintf(label, "%i", DPData.avail_shot);
    dialog_item_set_label(hMainDialog, 0x08, label, 1 + strlen(label), 0x26);

    iso_print(value, DPData.iso);
    dialog_item_set_label(hMainDialog, 0x08, value, 1 + strlen(value), 0x04);

    dialog_item_set_label(hMainDialog, 0x12, &symbol, 0x04, 0x09);

    display_refresh();
}

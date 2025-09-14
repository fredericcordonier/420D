#include <vxworks.h>

/**
 * @file viewfinder.c
 * @brief Actions while looking through the viewfinder.
 *
 * These actions have effect when the "display off" sensor is active.
 * This means that the display shall be ON, and the sensor activated by the
 * face.
 *
 * The features while looking through the viewfinder are:
 * - M mode:
 *   - SET: toggle the @ref fexp
 *   - RIGHT / LEFT: increase / decrease exposure compensation if @ref autoiso
 * is enabled
 *   - UP: Reset the exposure compensation if @ref autoiso is enabled
 *   - DOWN: Reset Av and TV values to match measured exposition, see @ref qexp
 * - Other expert modes (P, Av, Tv):
 *   - SET: Set the final exposure for @ref msm
 *   - RIGHT/LEFT: increase / decrease ISO value, see @ref iso_vf
 *   - UP: display ISO, see @ref iso_vf
 *   - DOWN: Take a measure for @ref msm
 */

/**
 * @defgroup iso_vf ISO in viewfinder
 *
 * When the "Use D-Pad" option is active (set to "Yes"), and if the camera is
 * set in any creative mode, you can change the ISO value while looking through
 * the viewfinder:
 * - Set the camera to a creative mode.
 * - Look through the viewfinder, and half-press the shutter button; you should
 * see the current measurements (shutter speed, aperture...).
 * - Now press the LEFT and RIGHT buttons to change the ISO value, or UP just to
 * display it; while the button is kept pressed down, the new value will be
 * displayed in the viewfinder, in the same place of other information.
 *
 * The ISO value is (temporarily) displayed in place of other information:
 * - In M or TV modes: the ISO is displayed in place of the selected speed
 * - In P or Av modes: the ISO is displayed in place of the burst counter
 * (because speed is managed by the camera). It is coded as: | Burst counter  |
 * ISO value | | ---------------|-----------:| | 1              |  100       |
 *   | 2              |  200       |
 *   | 4              |  400       |
 *   | 8              |  800       |
 *   | 9              | 1600       |
 */
#include "firmware.h"
#include "firmware/camera.h"

#include "macros.h"
#include "main.h"

#include "exposure.h"
#include "fexp.h"
#include "intercom.h"
#include "msm.h"
#include "persist.h"
#include "qexp.h"
#include "settings.h"
#include "utils.h"

#include "viewfinder.h"

dpr_data_t vf_DPData;

void viewfinder_change_iso(iso_t iso);
void viewfinder_display_iso(iso_t iso);

void viewfinder_change_evc(ec_t av_comp);

/**
 * @brief Handle Right button while looking through the viewfinder.
 *
 */
void viewfinder_right() {
    if (settings.autoiso_enable) {
        // AutoISO + M => change exposure compensation
        if (DPData.ae == AE_MODE_M)
            viewfinder_change_evc(ec_inc(persist.ev_comp, FALSE));
    } else {
        // Only for creative modes
        if (AE_IS_CREATIVE(DPData.ae))
            viewfinder_change_iso(iso_next(DPData.iso));
    }
}

/**
 * @brief Handle Right button while looking through the viewfinder.
 *
 */
void viewfinder_left() {
    if (settings.autoiso_enable) {
        // AutoISO + M => change exposure compensation
        if (DPData.ae == AE_MODE_M)
            viewfinder_change_evc(ec_dec(persist.ev_comp, FALSE));
    } else {
        // In Creative modes, decrease ISO while looking through the viewfinder
        if (AE_IS_CREATIVE(DPData.ae))
            viewfinder_change_iso(iso_prev(DPData.iso));
    }
}

/**
 * @brief Handle the UP button while looking through the viewfinder.
 *
 */
void viewfinder_up() {
    if (settings.autoiso_enable) {
        // AutoISO + M => reset exposure compensation
        if (DPData.ae == AE_MODE_M)
            viewfinder_change_evc(EV_ZERO);
    } else {
        // Only for creative modes
        if (AE_IS_CREATIVE(DPData.ae))
            viewfinder_display_iso(DPData.iso);
    }
}

/**
 * @brief Handle DOWN button while looking through the viewfinder.
 *
 */
void viewfinder_down() {
    switch (DPData.ae) {
    case AE_MODE_M:
        qexp_enable();
        break;
    case AE_MODE_P:
    case AE_MODE_TV:
    case AE_MODE_AV:
        msm_register();
        break;
    default:
        break;
    }
}

/**
 * @brief Handle SET button while looking through the viewfinder.
 *
 */
void viewfinder_set() {
    switch (DPData.ae) {
    case AE_MODE_M:
        fexp_toggle();
        break;
    case AE_MODE_P:
    case AE_MODE_TV:
    case AE_MODE_AV:
        msm_start();
        break;
    default:
        break;
    }
}

/**
 * @brief Manage the button release while viewing through the viewfinder.
 *
 */
void viewfinder_end() {
    switch (status.vf_status) {
    case (VF_STATUS_ISO):
        switch (DPData.ae) {
        case AE_MODE_M:
        case AE_MODE_TV:
            // Restore previous state
            send_to_intercom(IC_SET_CF_EMIT_FLASH, vf_DPData.cf_emit_aux);
            send_to_intercom(IC_SET_TV_VAL, vf_DPData.tv_val);

            // Do not reset VF_STATUS if Tv was not restored
            if (DPData.tv_val != vf_DPData.tv_val)
                return;

            break;
        case AE_MODE_P:
        case AE_MODE_AV:
            send_to_intercom(IC_SET_BURST_COUNTER, 9);
            break;
        default:
            break;
        }

        break;
    case (VF_STATUS_MSM):
        msm_release();
        break;
    case (VF_STATUS_FEXP):
        fexp_disable();
        break;
    case (VF_STATUS_QEXP):
        qexp_disable();
        break;
    default:
        break;
    }

    status.vf_status = VF_STATUS_NONE;
}

void viewfinder_change_iso(iso_t iso) {
    // Set new ISO
    send_to_intercom(IC_SET_ISO, iso);

    // Display new ISO
    viewfinder_display_iso(iso);
}

void viewfinder_display_iso(iso_t iso) {
    // Save current state
    vf_DPData = DPData;

    switch (DPData.ae) {
    // Display ISO as shutter speed
    case AE_MODE_M:
    case AE_MODE_TV:
        // Change to Tv=ISO, no flash
        send_to_intercom(IC_SET_CF_EMIT_FLASH, TRUE);
        send_to_intercom(IC_SET_TV_VAL, (iso & 0xF8) + 0x25);

        break;
    // Display ISO as burst counter
    case AE_MODE_P:
    case AE_MODE_AV:
        switch (iso) {
        case ISO_100:
            send_to_intercom(IC_SET_BURST_COUNTER, 1);
            break;
        case ISO_200:
            send_to_intercom(IC_SET_BURST_COUNTER, 2);
            break;
        case ISO_400:
            send_to_intercom(IC_SET_BURST_COUNTER, 4);
            break;
        case ISO_800:
            send_to_intercom(IC_SET_BURST_COUNTER, 8);
            break;
        case ISO_1600:
            send_to_intercom(IC_SET_BURST_COUNTER, 9);
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }

    // Set flag to restore viewfinder later
    status.vf_status = VF_STATUS_ISO;
}

void viewfinder_change_evc(ec_t ev_comp) {
    persist.ev_comp = CLAMP(ev_comp, EV_CODE(-2, 0), EV_CODE(2, 0));
    enqueue_action(persist_write);
}

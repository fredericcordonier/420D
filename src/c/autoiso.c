/** \file autoiso.c
 *
 * Implmentation of auto ISO functionality for M, Av, Tv and P modes.
 */

/**
 * @defgroup autoiso Auto ISO
 * @brief Implementation of auto ISO in expert modes.
 *
 * 400plus has support for AutoISO in creative modes, using our own algorithms.
 * It works by reading the measurements that the camera makes, when you
 * half-press the shutter button, and changing the ISO value accordingly. The
 * behavior of AutoISO is different for each creative mode, but the idea is
 * roughly the same: use the lowest possible ISO to maintain a suitable speed or
 * aperture.
 *
 * To enable AutoISO
 * - enter the ISO selection dialog
 * - Press DP
 *
 * To disable AutoISO, enter that dialog again and select any other ISO value.
 *
 * Alternatively, AutoISO can be configured in the "AutoISO" sub-menu in the
 * parameters page at
 * @ref menu :
 * | Item    | Description |
 * |---------|------------|
 * | Enabled | When active (set to "Yes"), AutoISO is enabled. | | Min ISO |
 * Minimum ISO value that AutoISO will use. | | Max ISO | Maximum ISO value that
 * AutoISO will use. | | Min Tv  | Minimum shutter speed that AutoISO will try
 * to maintain.                                                 | | Max Av  |
 * Maximum aperture (relative to maximum aperture supported by the lens) that
 * AutoISO will try to maintain. | | Relaxed | When active (set to "Yes"),
 * AutoISO in M mode will allow larger exposure deviations. |
 *
 * <H2>AutoISO In Av mode</H2>
 * The intention of AutoISO in Av mode is to use the lowest possible ISO that
 * ensures no motion blur. In normal Av mode, the user selects ISO and aperture,
 * and the camera decides the shutter speed according to the scene. But when
 * AutoISO is active, 400plus will always try to use the lowest possible ISO
 * value (as configured by "Min ISO"), unless the shutter speed gets too low (as
 * configured by "Min Tv"); when that happens, 400plus will automatically raise
 * the ISO value (up to "Max ISO") as needed.
 *
 * Please, note that when both Safety Shift and AutoISO are enabled, the camera
 * could drop shutter speed below the configured "Min Tv", if "Max ISO" is
 * reached, in order to maintain a correct exposure.
 *
 * AutoISO In Tv mode
 * In Tv mode, the intention of AutoISO is to use the lowest possible ISO that
 * ensures an acceptable depth of field.
 *
 * In Tv mode, AutoISO will try to keep ISO as low as possible (as configured by
 * "Min ISO"), unless the scene requires an aperture larger than desired (as
 * configured at "Max Av"); then, AutoISO will raise the ISO value (up to "Max
 * ISO") as needed. Please, note that the "Max Av" parameter is the difference
 * between the maximum aperture supported by your lens and the maximum aperture
 * that AutoISO will allow before raising the ISO value: for example, on a f/4
 * lens, setting "Max Av" to +1EV means that AutoISO should raise the ISO
 * whenever the camera sets an aperture of f/5.6 or greater. On a zoom lens with
 * a variable maximum aperture, "Max Av" is always based on the maximum aperture
 * supported by your lens at each moment.
 *
 * In Tv mode, it is perfectly possible to combine both Safety Shift and AutoISO
 * at the same time: when AutoISO can no longer increase the ISO value, the
 * camera will use a larger aperture, and when the lens' maximum aperture is
 * reached, Safety Shift will force a longer exposure time.
 */

#include <vxworks.h>

#include "firmware/camera.h"

#include "macros.h"
#include "main.h"

#include "display.h"
#include "exposure.h"
#include "intercom.h"
#include "persist.h"
#include "settings.h"
#include "utils.h"

#include "autoiso.h"

/**
 * @brief Compute the auto ISO in various cases.
 * @ingroup autoiso
 *
 * The implementation depends on the mode.
 * In M, try to match the selected Tv and Av to the measured Ev.
 * In other expert modes, change the ISO if Ev does not allow keeping the
 * selected values of Tv and Av.
 */
void autoiso() {
    ev_t limit;

    ec_t ec = EC_ZERO;
    ev_t ev = EV_ZERO;

    iso_t newiso;

    switch (DPData.ae) {
    case AE_MODE_M:
        if (DPData.tv_val != TV_VAL_BULB) {
            // M mode: set ISO to match exposure
            ec = -(status.measured_ec - persist.ev_comp);

            // Normalize an apply new ISO
            if (ec != EC_ZERO) {
                if (settings.autoiso_relaxed)
                    ec = (ec - 1) / 3;

                newiso = DPData.iso + ec;
                newiso = CLAMP(newiso, settings.autoiso_miniso,
                               settings.autoiso_maxiso);
                newiso = EV_ROUND(newiso);

                send_to_intercom(IC_SET_ISO, newiso);
            }
        }

        return;
    case AE_MODE_P:
    case AE_MODE_AV:
        // P / Av mode: raise ISO if shutter time is lower than limit
        ev = status.measured_tv;
        limit = settings.autoiso_mintv;
        break;
    case AE_MODE_TV:
        // Tv mode: raise ISO if apperture is larger than lens' max plus offset
        ev = status.measured_av;
        limit = DPData.avo + settings.autoiso_maxav;
        break;
    default:
        return;
    }

    // Decide whether we need to change current ISO
    if (ev != EC_ZERO) {
        if (ev < limit)
            ec = (limit - ev) + EV_CODE(1, 0);
        else if (ev >= limit + EV_CODE(1, 0))
            ec = (limit - ev);
    }

    // Normalize an apply new ISO
    if (ec != EC_ZERO) {
        newiso = DPData.iso + ec;
        newiso =
            CLAMP(newiso, settings.autoiso_miniso, settings.autoiso_maxiso);
        newiso = EV_TRUNC(newiso);

        send_to_intercom(IC_SET_ISO, newiso);
    }
}

/**
 * @brief Activate auto ISO
 * @ingroup autoiso
 */
void autoiso_enable() {
    if (DPData.ae == AE_MODE_M && DPData.tv_val == TV_VAL_BULB)
        return;

    press_button(IC_BUTTON_SET);

    if (!settings.autoiso_enable) {
        settings.autoiso_enable = TRUE;
        enqueue_action(settings_write);
    }

    print_icu_info();
    beep();
}

/**
 * @brief Disable the auto ISO.
 * @ingroup autoiso
 */
void autoiso_disable() {
    if (settings.autoiso_enable) {
        settings.autoiso_enable = FALSE;
        enqueue_action(settings_write);
    }
}

/**
 * @brief Restore the minimal ISO if in mode M and Bulb is selected as Tv.
 * @ingroup autoiso
 */
void autoiso_restore() {
    if (DPData.ae == AE_MODE_M && DPData.tv_val == TV_VAL_BULB) {
        send_to_intercom(IC_SET_ISO, ISO_MIN);
    }
}

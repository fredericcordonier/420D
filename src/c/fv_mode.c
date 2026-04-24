
#include "firmware.h"
#include "intercom.h"
#include "main.h"
#include "firmware/camera.h"
#include "fv_mode.h"
#include "persist.h"

/**
 *  Fv Mode is a mode introduced in recent Canon cameras. The goal of this mode is to automatically change
 * the actual exposure mode depending on what the user selected:
 *
 * - If the user keeps Tv and Av in Auto, the camera will be in P mode
 * - If the user selects a specific Tv or Av, the camera will be in Tv or Av mode
 * - If the user selects a specific Tv and Av, the camera will be in M mode
 *
 * There must be a value in settings_t indicating if the user wants this Fv mode in place of the full auto mode.
 *
 * We need to define a structure containing all data selected by the user and the measured values, and then update the selected Tv and Av to match the user selection.
 *
 * The structure shall contain:
 * - The Tv and Av values selected by the user (or Auto) (find a specific value for Auto)
 * - The current mode (P, Tv, Av, M) in which the camera is currently running.
 * - The Fv mode will be assigned to the physical "green square" mode (full auto), but the actual mode will be changed to P, Tv, Av or M
 *  depending on the user selection.
 * - The user selection will be done by the main dial and the Av/Tv buttons, as in normal operation. The only difference is that the camera will automatically
 * switch to the appropriate mode.
 *
 * In fv_mode_update, we will check the user selection and the measured values, and update the Tv and Av values to match the user selection.
 * 1. if the camera is in P mode and the user selects a specific Tv or Av, we will switch to Tv or Av mode and set the Tv or Av value to match the user selection. If he
 *    selects both, we will switch to M mode and set both values.
 * 2. if the camera is in Tv or Av mode and the user selects a specific Tv or Av, we will switch to M mode and set the other value to match the measured value.
 * 3. if the camera is in M mode and the user selects Auto for Tv or Av, we will switch to Tv or Av mode and set the Tv or Av value to match the user selection.
 * If he selects Auto for both, we will switch to P mode.
 *
 * Finally, we need to define which data will be persistent in flash memory so that at next startup, the camera will be in the same mode as before, with the same
 * user selection (Tv and Av values).
 */

void fv_mode_init(void) {
}

void fv_mode_apply(void) {
    // Only if dial mode is set to Green Square, we want to be in Fv mode
    if (status.main_dial_ae == AE_MODE_AUTO) {
        // Check user selection and update Tv and Av values accordingly
        if (persist.fv_av == FV_MODE_AV_AUTO && persist.fv_tv == FV_MODE_TV_AUTO) {
            // User selected Auto for both Tv and Av, switch to P mode
            if (DPData.ae != AE_MODE_P)
                send_to_intercom(IC_SET_AE, AE_MODE_P);
        } else if (persist.fv_av == FV_MODE_AV_AUTO) {
            // User selected Auto for Av, switch to Tv mode and set Tv value
            if (DPData.ae != AE_MODE_TV)
                send_to_intercom(IC_SET_AE, AE_MODE_TV);

            if (persist.fv_tv != DPData.tv_val)
                send_to_intercom(IC_SET_TV_VAL, persist.fv_tv);
        } else if (persist.fv_tv == FV_MODE_TV_AUTO) {
            // User selected Auto for Tv, switch to Av mode and set Av value
            if (DPData.ae != AE_MODE_AV)
                send_to_intercom(IC_SET_AE, AE_MODE_AV);

            if (persist.fv_av != DPData.av_val)
                send_to_intercom(IC_SET_AV_VAL, persist.fv_av);
        } else {
            // User selected specific values for both Tv and Av, switch to M mode and set both values
            if (DPData.ae != AE_MODE_M)
                send_to_intercom(IC_SET_AE, AE_MODE_M);

            if (persist.fv_tv != DPData.tv_val)
                send_to_intercom(IC_SET_TV_VAL, persist.fv_tv);

            if (persist.fv_av != DPData.av_val)
                send_to_intercom(IC_SET_AV_VAL, persist.fv_av);
        }
    }
}

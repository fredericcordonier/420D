/**
 * @file fexp.c
 * @brief Fix exposition in M mode
 */

/**
 * @defgroup fexp Fixed exposure in M mode
 * @brief Fix exposure in M mode.
 * 
 * While in M mode, 400plus allows you to fix the exposure to a certain value, and keep it stable 
 * even if you change the aperture or the exposure time: when you change on of those two values, 
 * 400plus will automatically change the other one, in the opposite direction.
 * 
 * For example, suppose you set the camera to 1/100s and f/4.0 and then activate this feature. 
 * Now, if you change the exposure time to 1/50s, the aperture will jump to f/5.6; and if
 * you change the aperture to f/2.8, the shutter speed will jump to 1/200s.
 * 
 * To use this feature, the "Use D-Pad" option must be active (set to "Yes"); then, look through the 
 * viewfinder and press the SET button. A small "#" symbol will be shown on the main display, just 
 * between the shutter speed and the aperture, to indicate that the feature is active. To deactivate 
 * it, just press SET again while looking through the viewfinder.
 * 
*/
#include <vxworks.h>

#include "firmware.h"
#include "firmware/camera.h"

#include "main.h"
#include "macros.h"
#include "exposure.h"
#include "utils.h"
#include "intercom.h"

#include "fexp.h"

/**
 * @brief Toggle the activation of fixed exposure.
 * @ingroup fexp
 * 
 */
void fexp_toggle(void) {
	if (status.vf_status == VF_STATUS_FEXP)
		fexp_disable();
	else
		fexp_enable();
}

/**
 * @brief Enable the fixed exposure.
 * @ingroup fexp
 * 
 */
void fexp_enable(void) {
	status.vf_status = VF_STATUS_FEXP;
	status.fexp_ev   = (int)DPData.av_val + (int)DPData.tv_val;
}

/**
 * @brief Disable the fixed exposure.
 * @ingroup fexp
 * 
 */
void fexp_disable(void) {
	status.vf_status = VF_STATUS_NONE;
}

/**
 * @brief Change the aperture for fixed exposure
 * @ingroup fexp
 * 
 */
void fexp_update_av(void) {
	if (DPData.ae == AE_MODE_M && DPData.tv_val != TV_VAL_BULB) {
		int av = status.fexp_ev - DPData.tv_val;

		av = CLAMP(av, AV_MIN, AV_MAX);
		av = ev_normalize(av);

		send_to_intercom(IC_SET_AV_VAL, av);
	}
}

/**
 * @brief Update speed for fixed exposure.
 * @ingroup fexp
 * 
 */
void fexp_update_tv(void) {
	if (DPData.ae == AE_MODE_M && DPData.tv_val != TV_VAL_BULB) {
		int tv = status.fexp_ev - DPData.av_val;

		tv = CLAMP(tv, TV_MIN, TV_MAX);
		tv = ev_normalize(tv);

		send_to_intercom(IC_SET_TV_VAL, tv);
	}
}

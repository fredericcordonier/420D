/**
 * @file button.c
 * @brief Management of buttons in all situations.
 * 
 */
#include <vxworks.h>

#include "firmware.h"

#include "main.h"
#include "macros.h"
#include "debug.h"

#include "actions.h"
#include "autoiso.h"
#include "af_patterns.h"
#include "display.h"
#include "fexp.h"
#include "qexp.h"
#include "menu.h"
#include "menu_main.h"
#include "settings.h"
#include "utils.h"
#include "viewfinder.h"
#include "shortcuts.h"

#include "button.h"

typedef struct {
	int      block;
	action_t action_press;
	action_t action_release;
} reaction_t;

typedef struct  {
	reaction_t **reaction;
	int         *condition;
} chain_t;

reaction_t
	reaction_main_dp      = {TRUE,  menu_main_start},
	reaction_main_disp    = {TRUE,  shortcut_disp} ,
	reaction_main_jump    = {TRUE,  shortcut_jump},
	reaction_main_trash   = {TRUE,  shortcut_trash},
	reaction_main_av      = {FALSE, toggle_img_format},
	reaction_main_up      = {FALSE, restore_iso},
	reaction_main_down    = {FALSE, restore_wb},
	reaction_main_left    = {FALSE, restore_metering}
;

reaction_t *button_actions_main[BUTTON_COUNT] = {
	[BUTTON_DP]          = &reaction_main_dp,
	[BUTTON_DISP]        = &reaction_main_disp,
	[BUTTON_JUMP]        = &reaction_main_jump,
	[BUTTON_TRASH]       = &reaction_main_trash,
	[BUTTON_AV]          = &reaction_main_av,
	[BUTTON_UP]          = &reaction_main_up,
	[BUTTON_DOWN]        = &reaction_main_down,
	[BUTTON_LEFT]        = &reaction_main_left,
};

reaction_t
	reaction_420D_dp          = {TRUE,  menu_event_dp},
	reaction_420D_disp        = {TRUE,  menu_event_disp},
	reaction_420D_jump        = {TRUE,  menu_event_jump},
	reaction_420D_play        = {TRUE,  menu_event_play},
	reaction_420D_trash       = {TRUE,  menu_event_trash},
	reaction_420D_wheel_left  = {TRUE,  menu_event_prev},
	reaction_420D_wheel_right = {TRUE,  menu_event_next},
	reaction_420D_zoom_out    = {TRUE,  menu_event_out},
	reaction_420D_zoom_in     = {TRUE,  menu_event_in},
	/** AV button has push and release actions (diplay/select main menu items) */
	reaction_420D_av          = {TRUE,  menu_event_av, menu_event_av_up},
	reaction_420D_set         = {TRUE,  menu_event_set},
	reaction_420D_up          = {TRUE,  menu_event_up},
	reaction_420D_down        = {TRUE,  menu_event_down},
	reaction_420D_right       = {TRUE,  menu_event_right},
	reaction_420D_left        = {TRUE,  menu_event_left}
;

/**
 * @brief Button actions in 400plus menu.
 * 
 */
reaction_t *button_actions_420D[BUTTON_COUNT] = {
	[BUTTON_DP]          = &reaction_420D_dp,
	[BUTTON_DISP]        = &reaction_420D_disp,
	[BUTTON_JUMP]        = &reaction_420D_jump,
	[BUTTON_PLAY]        = &reaction_420D_play,
	[BUTTON_TRASH]       = &reaction_420D_trash,
	[BUTTON_WHEEL_LEFT]  = &reaction_420D_wheel_left,
	[BUTTON_WHEEL_RIGHT] = &reaction_420D_wheel_right,
	[BUTTON_ZOOM_OUT]    = &reaction_420D_zoom_out,
	[BUTTON_ZOOM_IN]     = &reaction_420D_zoom_in,
	[BUTTON_AV]          = &reaction_420D_av,
	[BUTTON_SET]         = &reaction_420D_set,
	[BUTTON_UP]          = &reaction_420D_up,
	[BUTTON_DOWN]        = &reaction_420D_down,
	[BUTTON_RIGHT]       = &reaction_420D_right,
	[BUTTON_LEFT]        = &reaction_420D_left,
};

reaction_t
	reaction_shortcut_disp        = {TRUE,  shortcut_event_disp},
	reaction_shortcut_av          = {TRUE,  shortcut_event_av},
	reaction_shortcut_set         = {TRUE,  shortcut_event_set},
	reaction_shortcut_up          = {TRUE,  shortcut_event_up},
	reaction_shortcut_down        = {TRUE,  shortcut_event_down},
	reaction_shortcut_right       = {TRUE,  shortcut_event_right},
	reaction_shortcut_left        = {TRUE,  shortcut_event_left},
	reaction_shortcut_release     = {TRUE,  shortcut_event_end}
;

/**
 * @brief Actions during "shortcut" (while pressing a shortcut button=Trash/Jump)
 * 
 */
reaction_t *button_actions_shortcut[BUTTON_COUNT] = {
	[BUTTON_DISP]        = &reaction_shortcut_disp,
	[BUTTON_AV]          = &reaction_shortcut_av,
	[BUTTON_SET]         = &reaction_shortcut_set,
	[BUTTON_UP]          = &reaction_shortcut_up,
	[BUTTON_DOWN]        = &reaction_shortcut_down,
	[BUTTON_RIGHT]       = &reaction_shortcut_right,
	[BUTTON_LEFT]        = &reaction_shortcut_left,
	[BUTTON_RELEASE]     = &reaction_shortcut_release, ///< BUTTON_RELEASE is a pseudo button (release of any button)
};

/** Link DP button (blocking) to set Spot metering */
reaction_t
	reaction_meter_dp = {TRUE, set_metering_spot}
;

/**
 * @brief Reaction of button (DP) while setting the Metering mode.  
 * 
 */
reaction_t *button_actions_meter[BUTTON_COUNT] = {
	[BUTTON_DP] = &reaction_meter_dp,
};

/** Pressing DP while setting White balance sets the color Temperature set in menu */
reaction_t
	reaction_wb_dp = {TRUE, set_whitebalance_colortemp}
;

/**
 * @brief Reactions for buttons while setting the White Balance.
 * 
 */
reaction_t *button_actions_wb[BUTTON_COUNT] = {
	[BUTTON_DP] = &reaction_wb_dp,
};

reaction_t
	/** Pressing DP enables Auto ISO */
	reaction_iso_dp  = {TRUE,  autoiso_enable},
	/** Pressing SET disables Auto iso and is not blocking for other functions. */
	reaction_iso_set = {FALSE, autoiso_disable}
;

/**
 * @brief Reactions of buttons while setting the ISO value.
 * 
 */
reaction_t *button_actions_iso[BUTTON_COUNT] = {
	[BUTTON_DP]  = &reaction_iso_dp,
	[BUTTON_SET] = &reaction_iso_set,
};

/** Reactions while looking through the viewfinder */
reaction_t
	reaction_face_set   = {TRUE, viewfinder_set},
	/** The following reactions include a "button up" action to restore data display in WF */
	reaction_face_up    = {TRUE, viewfinder_up,    viewfinder_end},
	reaction_face_down  = {TRUE, viewfinder_down,  viewfinder_end},
	reaction_face_right = {TRUE, viewfinder_right, viewfinder_end},
	reaction_face_left  = {TRUE, viewfinder_left,  viewfinder_end}
;

/**
 * @brief Reactions of buttons when looking through the viewfinder.
 * 
 */
reaction_t *button_actions_face[BUTTON_COUNT] = {
	[BUTTON_SET]   = &reaction_face_set,
	[BUTTON_UP]    = &reaction_face_up,
	[BUTTON_DOWN]  = &reaction_face_down,
	[BUTTON_RIGHT] = &reaction_face_right,
	[BUTTON_LEFT]  = &reaction_face_left,
};

/** 
 * Actions to select the AF pattern.
*/
reaction_t
	reaction_af_set   = {TRUE, afp_center},
	reaction_af_up    = {TRUE, afp_top},
	reaction_af_down  = {TRUE, afp_bottom},
	reaction_af_right = {TRUE, afp_right},
	reaction_af_left  = {TRUE, afp_left},
	/** Block DISP button during selection of AF pattern */
	reaction_af_disp  = {TRUE}
;

/**
 * @brief List of reactions for buttons during AF pattern selection.
 * 
 */
reaction_t *button_actions_af[BUTTON_COUNT] = {
	[BUTTON_SET]   = &reaction_af_set,
	[BUTTON_UP]    = &reaction_af_up,
	[BUTTON_DOWN]  = &reaction_af_down,
	[BUTTON_RIGHT] = &reaction_af_right,
	[BUTTON_LEFT]  = &reaction_af_left,
	[BUTTON_DISP]  = &reaction_af_disp,
};

/**
 * @brief Reacion for Drive mode selection, non blocking
 * 
 * This reaction is used to manage the settings of remote control.
 */
reaction_t
	reaction_drive_set    = {FALSE, drivemode_set};

reaction_t *button_actions_drive[BUTTON_COUNT] = {
	[BUTTON_SET]   = &reaction_drive_set,
};

/**
 * @brief Definition of action chains (action table + optional condition)
 * 
 */
chain_t
	chain_actions_main     = {button_actions_main},
	chain_actions_meter    = {button_actions_meter},
	chain_actions_wb       = {button_actions_wb},
	chain_actions_iso      = {button_actions_iso},
	chain_actions_drive    = {button_actions_drive},
	chain_actions_420D  = {button_actions_420D},
	chain_actions_shortcut = {button_actions_shortcut},
	chain_actions_af       = {button_actions_af},
	chain_actions_face     = {button_actions_face, &settings.use_dpad}
;

/**
 * @brief Table indicating the action chain for each button.
 * 
 */
chain_t *button_chains[GUIMODE_COUNT] = {
	[GUIMODE_OLC]       = &chain_actions_main,
	[GUIMODE_OFF]       = &chain_actions_main,
	[GUIMODE_METER]     = &chain_actions_meter,
	[GUIMODE_WB]        = &chain_actions_wb,
	[GUIMODE_ISO]       = &chain_actions_iso,
	[GUIMODE_AFPATTERN] = &chain_actions_af,
	[GUIMODE_DRIVE]     = &chain_actions_drive,
	[GUIMODE_420D]   = &chain_actions_420D,
	[GUIMODE_FACE]      = &chain_actions_face,
	[GUIMODE_SHORTCUT]  = &chain_actions_shortcut,
};

/**
 * @brief Lists the buttons which can be held down fo actions.
 * 
 */
int can_hold[BUTTON_COUNT] = {
	[BUTTON_AV]    = TRUE,
	[BUTTON_UP]    = TRUE,
	[BUTTON_DOWN]  = TRUE,
	[BUTTON_RIGHT] = TRUE,
	[BUTTON_LEFT]  = TRUE,
};

/**
 * @brief Button handler
 * 
 * Functioning of button handler:
 * 1. Determine the GUI mode, or use fake GUI modes for special conditions:
 * 		- GUIMODE_FACE: if display has been shut down by face sensor
 * 		- GUIMODE_400PLUS: in case we are in the 400Plus menu
 * 		- GUIMODE_SHORTCUT: while we are pressing a button assigned to a shortcut (Jump/Trash)
 * 2. Get the action "chain" corresponding to this display mode (struct chain_t)
 * 		- chain indicates if the button action is linked to a condition
 * 		- chain is linked to the action on button (reaction) (struct reaction_t)
 * 3. Analyse reaction (action_pressed) and launch action if it exists
 * 4. Check if the button can be hold for an action (can_hold). If it is the case:
 * 		- status.button_down will keep the reference of the button being hold
 * 		- button_up_action and button_up_block will keep te data linked to this button  
 * @param button button_t ID of button (ID is an enum of 400plus)
 * @param is_button_down int Flag if the button is down or up
 * @return int Indication if the button blocks or not
 */
int button_handler(button_t button, int is_button_down) {
	static action_t   button_up_action = NULL;  // Action that must be executed when the current button is released
	static int        button_up_block  = FALSE; // Reaction when the current button is released

	guimode_t gui_mode;

	chain_t    *chain;
	reaction_t *reaction;

	// Check first for button-down events
	if (is_button_down) {
		// Use fictitious GUI modes so everything else fits nicely
		if (FLAG_FACE_SENSOR && FLAG_GUI_MODE == GUIMODE_OFF)
			gui_mode = GUIMODE_FACE;
		else if(status.menu_running)
			gui_mode = GUIMODE_420D;
		else if(status.shortcut_running != SHORTCUT_NONE)
			gui_mode = GUIMODE_SHORTCUT;
		else
			gui_mode = FLAG_GUI_MODE;


		if((chain = button_chains[gui_mode]) == NULL) {
			// This mode does not have an assigned chain
			//debug_log("gui_mode[0x%X]: no btn chain", gui_mode);
			return FALSE;
		} else if (!chain->condition || *chain->condition) {
			// Check that we have an action assigned to this button
			if ((reaction = chain->reaction[button]) == NULL) {
				//debug_log("gui_mode[0x%X]: btn[0x%X] no action", gui_mode, button);
				return FALSE;
			} else {
				// Launch the defined action
				if (reaction->action_press)
					enqueue_action(reaction->action_press);

				// Consider buttons with "button down" and "button up" events
				// and save "button up" parameters for later use
				if (can_hold[button]) {
					status.button_down = button;

					button_up_action = reaction->action_release;
					button_up_block  = reaction->block;
				}

				// Decide how to respond to this button
				return reaction->block;
			}
		} else {
			// This mode is configured off
			return FALSE;
		}
	} else {
		// Check for button-up events and act immediately
		if (status.button_down == button) {
			status.button_down = BUTTON_NONE;

			// Launch the defined action
			if (button_up_action)
				enqueue_action(button_up_action);

			// Decide how to respond to this button
			return button_up_block;
		}

		// If no action was found, do not block it
		return FALSE;
	}
}

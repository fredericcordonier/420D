#include <iostream>
#include <fstream>
#include <string.h>
#include "firmware.h"
#include "dirent.h"
// #include <firmware/fio.h>
#include <firmware/eventproc.h>
#include <vxworks/ioLib.h>
#include "ini_wrap.h"
#include "eos420d.h"

#include "main.h"
#include "settings.h"
#include "keypad.h"


// #include "firmware/eventproc.h"

// #include "macros.h"

#include "cache_hacks.h"
#include "display.h"
#include "intercom.h"
#include "persist.h"
#include "cmodes.h"
#include "debug.h"

#include <gtk/gtk.h>
#include "eos420d_window.h"

#ifdef __cplusplus
extern "C" {
#endif

class SimulFirmware : public FirmwareBridge {
public:
    SimulFirmware(EOS400DWindow*& uiRef) : ui(uiRef) {}
    void onButton(Button btn) override;
	void redraw(void) override;
    void setLCD(const std::array<std::string, LCD_LINES>& lines) override;
    void setFaceSensor(bool active);

private:
    EOS400DWindow*& ui;
};

void SimulFirmware::setFaceSensor(bool active) {
	FLAG_FACE_SENSOR = active;
	if (!active) {
		FLAG_GUI_MODE = GUIMODE_OFF;
	}
}

void SimulFirmware::onButton(Button btn) {
    std::map<Button, button_t>map_table;

	// BUTTON_JUMP,
	// BUTTON_DRIVE,

	map_table = {
		{Button::AV, BUTTON_AV},
		{Button::WHEEL_R, BUTTON_WHEEL_RIGHT},
		{Button::WHEEL_L, BUTTON_WHEEL_LEFT},
		{Button::UP, BUTTON_UP},
		{Button::LEFT, BUTTON_LEFT},
		{Button::SET, BUTTON_SET},
		{Button::RIGHT, BUTTON_RIGHT},
		{Button::DOWN, BUTTON_DOWN},
		{Button::DP, BUTTON_DP},
		{Button::MENU, BUTTON_MENU},
		{Button::TRASH, BUTTON_TRASH},
		{Button::PLAY, BUTTON_PLAY},
		{Button::ZOOM_IN, BUTTON_ZOOM_IN},
		{Button::ZOOM_OUT, BUTTON_ZOOM_OUT},
		{Button::INFO, BUTTON_DISP},
		// {'q', BUTTON_COUNT},
		{Button::SHUTTER_FULL, BUTTON_RELEASE},
	};
	std::map<Button, button_t>::iterator it = map_table.find(btn);
	if (it != map_table.end()) {
		my_camera.button_received(KeypadInput(btn, it->second));
	}
	else {
		my_camera.button_received(KeypadInput(btn, BUTTON_NONE));
	}
    std::array<std::string, LCD_LINES> screen;
    for (int item = 0; item < LCD_LINES; item++) {
        screen[item] = my_camera.display.get_display_line(item);
    }
	ui->updateLCD(screen);
}

void SimulFirmware::setLCD(const std::array<std::string, LCD_LINES>& lines) {
        ui->updateLCD(lines);
}

void SimulFirmware::redraw(void) {
    std::array<std::string, LCD_LINES> screen;
    for (int item = 0; item < LCD_LINES; item++) {
        screen[item] = my_camera.display.get_display_line(item);
    }
	ui->updateLCD(screen);
}

int ui_main(int argc, char* argv[])
{
    gtk_init(&argc, &argv);

    EOS400DWindow* ui = nullptr;
    SimulFirmware fw(ui);

    EOS400DWindow window(fw);
    ui = &window;

	my_camera.display.set_fw(&fw);

    window.show();
    gtk_main();

    return 0;
}

/**
 * Main message queue
 */
int *action_queue;

/**
 * Global status
 */
status_t status = {
	button_down       : BUTTON_NONE,
	script_running    : FALSE,
	script_stopping   : FALSE,
	menu_running      : FALSE,
	shortcut_running  : SHORTCUT_NONE,
	afp_dialog        : FALSE,
	measuring         : FALSE,
	msm_count         : 0,
	msm_tv            : EV_ZERO,
	msm_av            : EV_ZERO,
	ignore_msg        : IC_NONE,
	vf_status         : VF_STATUS_NONE,
	lock_redraw       : FALSE,
};

void hack_relocate   (void);
void cache_hacks     (void);

void disable_cache_clearing (void);

void hack_dmProcInit                (void);
int  hack_register_gui_idle_handler (void *org_proc, int zero);
int  hack_init_intercom_data        (void *old_proc);
void hack_StartConsole              (void);

void hack_pre_init_hook  (void);
void hack_post_init_hook (void);

void hack_jump_trash_events (int r0, int r1, int button);

void action_dispatcher(void);

int check_create_folder(void);

int main(int argc, char *argv[]) {
	// If TRASH button is pressed, do not initialize 420D at all
	if (BTN_TRASH != BTN_PRESSED) {
		// Switch blue LED on, it will be switched back off after initialization
		LEDBLUE = LEDON;

		// COPY the hack to our memory --> not doable on simulation
		// hack_relocate();

		// hack the caches
		cache_hacks();

	}

	ofw_entry_point(argc, argv);

	return 0;
}

/**
 * \brief 0xAF: check the devinfo for more details on why this routine is needed
 *
 */
void hack_relocate(void) {
}

/**
 * \brief Install the hacks
 *
 * After flushing and locking the caches, install 400plus own hooks.
 *
 * - hack_dmProcInit
 * - hack_init_intercom_data
 * - hack_StartConsole
 */
void cache_hacks(void) {
	hack_dmProcInit();

	hack_init_intercom_data(NULL);

	hack_StartConsole();
}

void disable_cache_clearing(void) {
}

/**
 * \brief Hook for dmProcInit.
 *
 * First call the FW dmProcInit, then our own init function.
 */
void hack_dmProcInit(void) {
	dmProcInit();

#ifdef ENABLE_MASSIVE_DEBUG
	// the 2nd level is 32 flags for debug classes
	// the 3rd arg is log level, 0 == full debug, >0 == less debug
	dmSetStoreLevel(hDbgMgr, 0xFF, 0);
	dmSetPrintLevel(hDbgMgr, 0xFF, 0);
#endif

	hack_pre_init_hook();
}

int hack_register_gui_idle_handler(void * org_proc, int zero) {
	return CreateCtrlMain(&hack_GUI_IDLEHandler, zero);
}

int hack_init_intercom_data(void *old_proc) {
	return InitIntercomData(intercom_proxy);
}

void hack_StartConsole(void) {
	hack_post_init_hook();

	// StartConsole does not return
	StartConsole();
}

void hack_pre_init_hook(void) {
	action_queue = (int*)CreateMessageQueue("action_queue", 0x40);
	CreateTask("Action Dispatcher", 25, 0x2000, action_dispatcher, 0);

}

// we can run extra code at the end of the OFW's task init
/**
 * \brief This function installs a button handler.
 */
void hack_post_init_hook(void) {
	SetSendButtonProc(&hack_jump_trash_events, 0);
}

/**
 * \brief Handler for buttons "Jump" and "Trash"
 */
void hack_jump_trash_events(int r0, int r1, int button) {
	switch (button) {
	case 4: // JUMP_UP
		button_handler(BUTTON_RELEASE, TRUE);
		break;
	case 5: // JUMP_DOWN
		button_handler(BUTTON_JUMP, TRUE);
		break;
	case 8: // TRASH_UP
		button_handler(BUTTON_RELEASE, TRUE);
		break;
	case 9: // TRASH_DOWN
		button_handler(BUTTON_TRASH, TRUE);
		break;
	}
}

// Our own thread uses this dispatcher to execute tasks

void action_dispatcher(void) {
	action_t action;

	// Loop while receiving messages
	for (;;) {
		ReceiveMessageQueue(action_queue, &action, FALSE);
		action();
	}
}

void enqueue_action(action_t action) {
	TryPostMessageQueue(action_queue, (action), FALSE);
}

void start_up() {
	// Check and create our 420D folder
	status.folder_exists = check_create_folder();

	// Recover persisting information
	persist_read();

	// Read settings from file
	settings_read();

	// If configured, start debug mode
	if (settings.debug_on_poweron)
		start_debug_mode();

	// If configured, restore AEB
	if (settings.persist_aeb)
		send_to_intercom(IC_SET_AE_BKT, persist.aeb);

	// Enable IR remote
	// i'm not sure where to call this? perhaps this isn't the right place.
	if (settings.remote_enable)
		remote_on();

	// Enable extended ISOs
	// Enable (hidden) CFn.8 for ISO H
	send_to_intercom(IC_SET_CF_EXTEND_ISO, 1);

	// Enable realtime ISO change
	send_to_intercom(IC_SET_REALTIME_ISO_0, 0);
	send_to_intercom(IC_SET_REALTIME_ISO_1, 0);

	// Set current language
	enqueue_action(lang_pack_init);

	// Read custom modes configuration from file
	enqueue_action(cmodes_read);

	// And optionally apply a custom mode
	enqueue_action(cmode_recall);

    // turn off the blue led after it was lighten by our hack_task_MainCtrl()
	eventproc_EdLedOff();

#ifdef MEMSPY
	debug_log("starting memspy task");
	CreateTask("memspy", 0x1e, 0x1000, memspy_task, 0);
#endif

}

/*
 * Look for a "420D" folder, and create it if it does not exist
 */

int check_create_folder(void)
{
    DIR *dirp;

	if ((dirp = opendir(FOLDER_PATH)) == NULL) {
		if(FIO_CreateDirectory(FOLDER_PATH)) {
			return FALSE;
		} else {
			return TRUE;
		}
    } else {
    	closedir(dirp);
    	return TRUE;
    }
}

int ofw_entry_point(int argc, char *argv[]) {
    std::cout << "Hello from UT main" << std::endl;
    int result = settings_read();
    std::cout << "Result of settings_read(): " << result << std::endl;
	// unsigned char button_msg[4] = {4, IC_BUTTON_UP, FALSE, 0};
	// my_camera.start();
	ui_main(argc, argv);
    settings_write();
    return 0;
}

#ifdef __cplusplus
}
#endif


#include <iostream>
#include <cstring>
#include <cstdint>
#include <firmware/fio.h>
#include "eos420d.h"
#include "utils.h"
#include "firmware.h"
#include "main.h"
#include "button.h"
#include "settings.h"
#include "firmware/gui.h"
#include "intercom.h"
#include "menu.h"


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

int FLAG_RELEASE_COUNT;
int  BodyID;
const char FIRMWARE_VERSION[] = "1.2.0";
const char OWNER_NAME[] = "Fred";
int BTN_TRASH = 0x00;
int LEDRED;
int LEDBLUE;


int SendToIntercom(int message, int length, int parm) {
	my_camera.intercom_420d.send(message, length, parm);
	return 0;
}

void SleepTask(long msec) {
	std::cout << "SleepTask: " << msec << std::endl;
	// Reset the button to make menus work
	status.button_down = BUTTON_NONE;
}

int DisplayOn;
int FaceStatus;
int shutter_lock;
dialog_t *hInfoCreative;
dialog_t *hMainDialog;
dialog_t *CreateDialogBox(int parm1, int parm2, event_handler_t handler, int tmpl, int is_busy_maybe)
{
	std::cout << "CreateDialogBox: " << parm1 << std::endl;
	return NULL;
}
int       DeleteDialogBox(dialog_t *dialog)
{
	std::cout << "DeleteDialogBox: " <<  std::endl;
	return 0;
}
void dialog_redraw        (dialog_t *dialog)
{
	my_camera.display.redraw();
	std::cout << "dialog_item_set_int: " <<  std::endl;
}
void dialog_item_set_int  (dialog_t *dialog, const int item, const int   data)
{
	std::cout << "dialog_item_set_int: " <<  std::endl;
}

char prefixes[10] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
void dialog_item_set_str  (dialog_t *dialog, const int item, const char *data)
{
	my_camera.display.print_line(item, data);
}

void dialog_item_set_label(dialog_t *dialog, const int type, const void *data, const int length, const int item)
{}
int dialog_event_handler (dialog_t *dialog, int *r1, gui_event_t event, int *r3, int r4, int r5, int r6, int code)
{
	std::cout << "dialog_event_handler: " <<  std::endl;
	return 0;
}
int olc_event_handler    (dialog_t *dialog, int *r1, gui_event_t event, int *r3, int r4, int r5, int r6, int code)
{return 0;}
void GUI_Select_Item  (void *menu_handle, int menu_item)
{}

void GUI_Highlight_Sub(void *menu_handle, int menu_item, int enable)
{
	my_camera.display.highlight_line(menu_item, TRUE);
}

void GUI_Disable_Item (void *menu_handle, int menu_item, int enable)
{
	std::cout << "GUI_Disable_Item: " <<  std::endl;
}
void window_instance_redraw(window_t *window)
{
	std::cout << "window_instance_redraw: " <<  std::endl;
}

// Switch display on / off

int SetTurnDisplayEvent_1_after_2(void)
{return 0;}
int SetTurnDisplayEvent_2_after_1(void)
{return 0;}

// Palette

void GUI_PaletteInit   (void)
{}
void GUI_PaletteUnInit (void)
{}
void GUI_PalettePush   (void)
{}
void GUI_PalettePop    (void)
{}
int  GUI_PaletteChange (int pallete)
{return 0;}

// Other

void GUI_Lock         (void)
{}
void GUI_UnLock       (void)
{}
void GUI_ClearImage   (void)
{}
void GUI_RefreshImage (void)
{
	std::cout << "GUI_RefreshImage: " << std::endl;
}
void GUI_StartMode    (guimode_t mode)
{
	std::cout << "GUI_StartMode: " << mode << std::endl;
}
int GUI_IDLEHandler(int unk0, int event, int unused, int unk1)
{
	return 0;
}
int* CreateTask(const char *name, int prio, int stack_size, void (*entry)(void), long parm)
{ static int task_nb = 0;
	return &task_nb;}
void UnSuspendTask(int *task)
{}
void SuspendTask(int *task)
{
}

int able_to_release(void) {
	return 1;
}
int EnterFactoryMode(void)
{ return 1;}
int ExitFactoryMode(void)
{ return 1;}
int IntercomHandlerButton(int button, int unknown)
{
	std::cout << "IntercomHandlerButton: " << button << std::endl;
	return 1;
}

void    ioGlobalStdSet (int stdFd, int newFd)
{}

int dumpf(void)
{ return 1;}

void flush_caches( void )
{

}

void cache_lock(void)
{

}

uint32_t cache_fake(uint32_t address, uint32_t data, uint32_t type)
{
	return 0;
}

int dmProcInit(void)
{
	return 0;
}

int StartConsole(void)
{
	return 0;
}

int SetSendButtonProc(void (*proc)(int, int, int), int unknown)
{
	return 0;
}

int CreateCtrlMain(int (*handler)(int, int, int, int), int zero)
{
	return 0;
}
int InitIntercomData(void (*proxy)(const int, unsigned char *))
{
	std::cout << "InitIntercomData: calling intercom for startup" << std::endl;
	unsigned char msg[2] = {(unsigned char)1, (unsigned char)IC_UNKNOWN_8D};
	intercom_proxy(0, msg);
	return 0;
}

int IntercomHandler(const int handler, const char *message)
{
	int len = (int)*message;
	std::cout << "IntercomHandler: " << handler << ":" << std::hex;
	for (int i=0; i < len; i++) {
		std::cout << (int)message[i];
	}
	std::cout << std::dec << std::endl;
	return 0;
}

int *CreateMessageQueue(const char *nameMessageQueue, int param)
{
	return 0;
}
int  ReceiveMessageQueue(void *hMessageQueue, void *pMessage, int forever)
{
	return 0;
}
int  TryPostMessageQueue(void *hMessageQueue, action_t pMessage, int forever)
{
	my_camera.post_message(pMessage);
	return 0;
}
int  hack_GUI_IDLEHandler(int unk0, int event, int unused, int unk1)
{
	return 0;
}

#ifdef __cplusplus
}
#endif // __cplusplus

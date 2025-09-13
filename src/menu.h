#ifndef MENU_H_
#define MENU_H_

#include "firmware/camera.h"

#define MENU_WAIT_FW 100

#define MENU_WIDTH  27
#define MENU_HEIGHT  5

#define AUTOREPEAT_DELAY_UNIT   50
#define AUTOREPEAT_DELAY_LONG   10
#define AUTOREPEAT_DELAY_SHORT   4

typedef enum {
	MENU_COLOR_RED              = 0x00,
	MENU_COLOR_BLUE             = 0x01,
	MENU_COLOR_YELLOW           = 0x02,
	MENU_COLOR_YELLOW_AND_WHITE = 0x06,
	MENU_COLOR_DARK_BLUE        = 0x09,
	//MENU_COLOR_SOMETHING_FOR_THE_HELP_SCREEN_MAY_BE = 0x0A, // cannot be used for menus
	MENU_COLOR_DARK_RED         = 0x0C,
	MENU_COLOR_YELLOW_AND_GREEN = 0x10, // nice
	MENU_COLOR_WHITE_ON_BLACK   = 0x12, // dark one
	MENU_COLOR_ORANGE           = 0x67,
} menu_color_t;

typedef struct menu_t     menu_t;
typedef struct menuitem_t menuitem_t;
typedef struct menupage_t menupage_t;

typedef enum {
	MENU_EVENT_MENU,
	MENU_EVENT_DISP,
	MENU_EVENT_JUMP,
	MENU_EVENT_PLAY,
	MENU_EVENT_TRASH,
	MENU_EVENT_SET,
	MENU_EVENT_PREV,
	MENU_EVENT_NEXT,
	MENU_EVENT_UP,
	MENU_EVENT_DOWN,
	MENU_EVENT_RIGHT,
	MENU_EVENT_LEFT,
	MENU_EVENT_DP,
	MENU_EVENT_AV,
	MENU_EVENT_AV_UP,
	MENU_EVENT_OUT,
	MENU_EVENT_IN,
	MENU_EVENT_OPEN,
	MENU_EVENT_DISPLAY,
	MENU_EVENT_REFRESH,
	MENU_EVENT_FINISH,
	MENU_EVENT_SAVE,
	MENU_EVENT_COUNT,
	MENU_EVENT_FIRST = 0,
	MENU_EVENT_LAST  = MENU_EVENT_COUNT - 1
} menu_event_t;

typedef void(*menuaction_t)(menu_t *menu);

typedef struct {
	int          size;
	menupage_t **data;
} list_page_t;

struct menu_t {
	menu_color_t   color;
	list_page_t    pages;
	menuaction_t   actions[MENU_EVENT_COUNT];
	int           *ordering;
	int            current_posn;
	menupage_t    *current_page;
	int            changed;
};

extern dpr_data_t menu_DPData;

void menu_create (menu_t * menu);
void menu_close  (void);
void menu_finish (menu_t *menu);
void menu_return (menu_t *menu);

void menu_set_posn(int posn);
void menu_set_page(menupage_t *page);

void menu_redraw   (void);
void menu_highlight(const int line);
void menu_set_text (const int line, const char *text);

void menu_event_menu   (void);
void menu_event_disp   (void);
void menu_event_jump   (void);
void menu_event_play   (void);
void menu_event_trash  (void);
void menu_event_set    (void);
void menu_event_prev   (void);
void menu_event_next   (void);
void menu_event_up     (void);
void menu_event_down   (void);
void menu_event_right  (void);
void menu_event_left   (void);
void menu_event_dp     (void);
void menu_event_av     (void);
void menu_event_av_up  (void);
void menu_event_out    (void);
void menu_event_in     (void);
void menu_event_open   (void);
void menu_event_display(void);
void menu_event_refresh(void);
void menu_event_finish (void);
void menu_event_save   (void);

void menu_event(menu_event_t);

void menu_set  (menu_t *menu);
void menu_right(menu_t *menu);
void menu_left (menu_t *menu);

void menu_next(menu_t *menu);
void menu_prev(menu_t *menu);

void menu_repeat(menu_t *menu, void (*action)(menu_t *menu, const int repeating));

#endif /* MENU_H_ */

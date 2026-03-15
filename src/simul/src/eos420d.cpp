#include <iostream>
#include <map>
#include <cstring>
#include "firmware/camera.h"
#include "firmware/gui.h"
#include "intercom.h"
#include "af_patterns.h"
#include "eos420d.h"
#include "eos420d_window.h"


EOS420D my_camera;
// Not inside EOS420D class, because accessed by firmware code
int GUIMode;

int RemReleaseInstMin;
int RemReleaseSelfMax;
dpr_data_t   DPData;

Display::Display() {
    for (int item = 0; item < LCD_LINES; item++) {
        highlight[item] = ' ';
    }
}

void Display::set_fw(FirmwareBridge *fw) {
    firmware = fw;
}

void Display::print_line(const int item, const char *data) {
    if (item == 8) {
        // Menu header
        display[0] = std::string(data, strlen(data));
    }
    if ((item > 0) && (item < LCD_LINES)) {
        display[item] = std::string(data, strlen(data));
    }
}
void Display::highlight_line(const int item, const int enable) {
    for (int line = 0; line < LCD_LINES; line++) {
        highlight[line] = (item == line) ? '*': ' ';
    }
}

std::string Display::get_display_line(const int item) {
    if (item < LCD_LINES) {
        std::string ret = std::string(1, highlight[item]);
        ret += " " + display[item];
        return ret;
    }
    else {
        return "";
    }
}

void Display::clear(void) {
    for (int item = 0; item < LCD_LINES; item++) {
        highlight[item] = ' ';
        display[item] = std::string();
    }
}

void Display::redraw(void) {
    firmware->redraw();
}

typedef std::map<int, std::string> ic_message_map;
ic_message_map ic_message = {
    // {IC_SET_AE, std::string("IC_SET_AE")},
	// {IC_SET_AF_POINT, std::string("IC_SET_AF_POINT")}
    {IC_SET_AE                      , std::string("IC_SET_AE                      ")},
    {IC_SET_METERING                , std::string("IC_SET_METERING                ")},
    {IC_SET_EFCOMP                  , std::string("IC_SET_EFCOMP                  ")},
    {IC_SET_DRIVE                   , std::string("IC_SET_DRIVE                   ")},
    {IC_SET_WB                      , std::string("IC_SET_WB                      ")},
    {IC_SET_AF                      , std::string("IC_SET_AF                      ")},
    {IC_SET_AF_POINT                , std::string("IC_SET_AF_POINT                ")},
    {IC_SET_TV_VAL                  , std::string("IC_SET_TV_VAL                  ")},
    {IC_SET_AV_VAL                  , std::string("IC_SET_AV_VAL                  ")},
    {IC_SET_AV_COMP                 , std::string("IC_SET_AV_COMP                 ")},
    {IC_SET_ISO                     , std::string("IC_SET_ISO                     ")},
    {IC_SET_RED_EYE                 , std::string("IC_SET_RED_EYE                 ")},
    {IC_SET_AE_BKT                  , std::string("IC_SET_AE_BKT                  ")},
    {IC_SET_WB_BKT                  , std::string("IC_SET_WB_BKT                  ")},
    {IC_SET_BEEP                    , std::string("IC_SET_BEEP                    ")},
    {IC_SET_COLOR_TEMP              , std::string("IC_SET_COLOR_TEMP              ")},
    {IC_SET_AUTO_POWER_OFF          , std::string("IC_SET_AUTO_POWER_OFF          ")},
    {IC_SET_VIEW_TYPE               , std::string("IC_SET_VIEW_TYPE               ")},
    {IC_SET_REVIEW_TIME             , std::string("IC_SET_REVIEW_TIME             ")},
    {IC_SET_AUTO_ROTATE             , std::string("IC_SET_AUTO_ROTATE             ")},
    {IC_SET_LCD_BRIGHTNESS          , std::string("IC_SET_LCD_BRIGHTNESS          ")},
    {IC_SET_DATE_TIME               , std::string("IC_SET_DATE_TIME               ")},
    {IC_SET_FILE_NUMBERING          , std::string("IC_SET_FILE_NUMBERING          ")},
    {IC_SET_LANGUAGE                , std::string("IC_SET_LANGUAGE                ")},
    {IC_SET_VIDEO_SYSTEM            , std::string("IC_SET_VIDEO_SYSTEM            ")},
    {IC_SET_HISTOGRAM               , std::string("IC_SET_HISTOGRAM               ")},
    {IC_SET_COLOR_SPACE             , std::string("IC_SET_COLOR_SPACE             ")},
    {IC_SET_IMG_FORMAT              , std::string("IC_SET_IMG_FORMAT              ")},
    {IC_SET_IMG_SIZE                , std::string("IC_SET_IMG_SIZE                ")},
    {IC_SET_IMG_QUALITY             , std::string("IC_SET_IMG_QUALITY             ")},
    {IC_SET_WBCOMP_GM               , std::string("IC_SET_WBCOMP_GM               ")},
    {IC_SET_WBCOMP_AB               , std::string("IC_SET_WBCOMP_AB               ")},
    {IC_SET_CF_SET_BUTTON_FUNC      , std::string("IC_SET_CF_SET_BUTTON_FUNC      ")},
    {IC_SET_CF_NR_FOR_LONG_EXPOSURE , std::string("IC_SET_CF_NR_FOR_LONG_EXPOSURE ")},
    {IC_SET_CF_EFAV_FIX_X           , std::string("IC_SET_CF_EFAV_FIX_X           ")},
    {IC_SET_CF_AFAEL_ACTIVE_BUTTON  , std::string("IC_SET_CF_AFAEL_ACTIVE_BUTTON  ")},
    {IC_SET_CF_EMIT_AUX             , std::string("IC_SET_CF_EMIT_AUX             ")},
    {IC_SET_CF_EXPLEVEL_INC_THIRD   , std::string("IC_SET_CF_EXPLEVEL_INC_THIRD   ")},
    {IC_SET_CF_EMIT_FLASH           , std::string("IC_SET_CF_EMIT_FLASH           ")},
    {IC_SET_CF_EXTEND_ISO           , std::string("IC_SET_CF_EXTEND_ISO           ")},
    {IC_SET_CF_AEB_SEQUENCE         , std::string("IC_SET_CF_AEB_SEQUENCE         ")},
    {IC_SET_CF_SI_INDICATE          , std::string("IC_SET_CF_SI_INDICATE          ")},
    {IC_SET_CF_MENU_POS             , std::string("IC_SET_CF_MENU_POS             ")},
    {IC_SET_CF_MIRROR_UP_LOCK       , std::string("IC_SET_CF_MIRROR_UP_LOCK       ")},
    {IC_SET_CF_FPSEL_METHOD         , std::string("IC_SET_CF_FPSEL_METHOD         ")},
    {IC_SET_CF_FLASH_METERING       , std::string("IC_SET_CF_FLASH_METERING       ")},
    {IC_SET_CF_FLASH_SYNC_REAR      , std::string("IC_SET_CF_FLASH_SYNC_REAR      ")},
    {IC_SET_CF_SAFETY_SHIFT         , std::string("IC_SET_CF_SAFETY_SHIFT         ")},
    {IC_SET_CF_LENS_BUTTON          , std::string("IC_SET_CF_LENS_BUTTON          ")},
    {IC_SET_CF_TFT_ON_POWER_ON      , std::string("IC_SET_CF_TFT_ON_POWER_ON      ")},
    {IC_SET_CF_QR_MAGNIFY           , std::string("IC_SET_CF_QR_MAGNIFY           ")},
    {IC_SET_CF_ORIGINAL_EVAL        , std::string("IC_SET_CF_ORIGINAL_EVAL        ")},
    {IC_DIALOGON                    , std::string("IC_DIALOGON                    ")},
    {IC_DIALOGOFF                   , std::string("IC_DIALOGOFF                   ")},
    {IC_SHUTDOWN                    , std::string("IC_SHUTDOWN                    ")},
    {IC_RELEASE                     , std::string("IC_RELEASE                     ")},
    {IC_RELEASE_INHIBIT             , std::string("IC_RELEASE_INHIBIT             ")},
    {IC_RELEASE_PERMIT              , std::string("IC_RELEASE_PERMIT              ")},
    {IC_RESET_SETTINGS              , std::string("IC_RESET_SETTINGS              ")},
    {IC_ENTERFACTORYMODE            , std::string("IC_ENTERFACTORYMODE            ")},
    {IC_EXITFACTORYMODE             , std::string("IC_EXITFACTORYMODE             ")},
    {IC_UILOCK                      , std::string("IC_UILOCK                      ")},
    {IC_GUILOCK                     , std::string("IC_GUILOCK                     ")},
    {IC_FIRMUP                      , std::string("IC_FIRMUP                      ")},
	// 0x64 ?
    {IC_BAT_TYPE                    , std::string("IC_BAT_TYPE                    ")},
    {IC_BC_LEVEL                    , std::string("IC_BC_LEVEL                    ")},
	// 0x67 ?
    {IC_CARD_FULL_MSG_IN_VF         , std::string("IC_CARD_FULL_MSG_IN_VF         ")},
    {IC_CHANGE_BAT_AND_OTHER_DIALOGS, std::string("IC_CHANGE_BAT_AND_OTHER_DIALOGS")},
    {IC_SET_BURST_COUNTER           , std::string("IC_SET_BURST_COUNTER           ")},
    {IC_HELLO_WORD                  , std::string("IC_HELLO_WORD                  ")},
    {IC_INITIALIZATION              , std::string("IC_INITIALIZATION              ")},
    {IC_POWER_FLAG                  , std::string("IC_POWER_FLAG                  ")},
    {IC_BUZZER                      , std::string("IC_BUZZER                      ")},
    {IC_TEMP                        , std::string("IC_TEMP                        ")},
    {IC_MEASURING                   , std::string("IC_MEASURING                   ")},
    {IC_MEASUREMENT                 , std::string("IC_MEASUREMENT                 ")},
    {IC_SHOOT_START                 , std::string("IC_SHOOT_START                 ")},
    {IC_SHOOT_FINISH                , std::string("IC_SHOOT_FINISH                ")},
    {IC_TIMERCOUNTER                , std::string("IC_TIMERCOUNTER                ")},
    {IC_UNKNOWN_8D                  , std::string("IC_UNKNOWN_8D                  ")},
    {IC_UNKNOWN_8E                  , std::string("IC_UNKNOWN_8E                  ")},
    {IC_SETTINGS_0                  , std::string("IC_SETTINGS_0                  ")},
    {IC_SETTINGS_1                  , std::string("IC_SETTINGS_1                  ")},
    {IC_SETTINGS_2                  , std::string("IC_SETTINGS_2                  ")},
    {IC_SETTINGS_3                  , std::string("IC_SETTINGS_3                  ")},
    {IC_BUTTON_MENU                 , std::string("IC_BUTTON_MENU                 ")},
    {IC_BUTTON_DISP                 , std::string("IC_BUTTON_DISP                 ")},
    {IC_BUTTON_JUMP                 , std::string("IC_BUTTON_JUMP                 ")},
    {IC_BUTTON_PLAY                 , std::string("IC_BUTTON_PLAY                 ")},
    {IC_BUTTON_TRASH                , std::string("IC_BUTTON_TRASH                ")},
    {IC_BUTTON_SET                  , std::string("IC_BUTTON_SET                  ")},
    {IC_AFPDLGOFF                   , std::string("IC_AFPDLGOFF                   ")},
	// and 0xA8 is perhaps dialog create.
    {IC_BUTTON_WHEEL                , std::string("IC_BUTTON_WHEEL                ")},
    {IC_BUTTON_WHEEL1               , std::string("IC_BUTTON_WHEEL1               ")},
    {IC_BUTTON_WHEEL_LEFT           , std::string("IC_BUTTON_WHEEL_LEFT           ")},
    {IC_BUTTON_WHEEL_RIGHT          , std::string("IC_BUTTON_WHEEL_RIGHT          ")},
    {IC_BUTTON_CARD_DOOR            , std::string("IC_BUTTON_CARD_DOOR            ")},
    {IC_BUTTON_UNK1                 , std::string("IC_BUTTON_UNK1                 ")},
    {IC_BUTTON_POWER                , std::string("IC_BUTTON_POWER                ")},
    {IC_BUTTON_BATTERY_DOOR         , std::string("IC_BUTTON_BATTERY_DOOR         ")},
    {IC_BUTTON_UP                   , std::string("IC_BUTTON_UP                   ")},
    {IC_BUTTON_DOWN                 , std::string("IC_BUTTON_DOWN                 ")},
    {IC_BUTTON_RIGHT                , std::string("IC_BUTTON_RIGHT                ")},
    {IC_BUTTON_LEFT                 , std::string("IC_BUTTON_LEFT                 ")},
    {IC_BUTTON_HALF_SHUTTER         , std::string("IC_BUTTON_HALF_SHUTTER         ")},
    {IC_BUTTON_FULL_SHUTTER         , std::string("IC_BUTTON_FULL_SHUTTER         ")},
    {IC_BUTTON_DP                   , std::string("IC_BUTTON_DP                   ")},
    {IC_AFPDLGON                    , std::string("IC_AFPDLGON                    ")},
    {IC_BUTTON_DRIVE                , std::string("IC_BUTTON_DRIVE                ")},
    {IC_BUTTON_AV                   , std::string("IC_BUTTON_AV                   ")},
    {IC_BUTTON_UNK2                 , std::string("IC_BUTTON_UNK2                 ")},
    {IC_MONOPEN                     , std::string("IC_MONOPEN                     ")},
    {IC_MONCLOSE                    , std::string("IC_MONCLOSE                    ")},
    {IC_MONREAD                     , std::string("IC_MONREAD                     ")},
    {IC_MONWRITE                    , std::string("IC_MONWRITE                    ")},
    {IC_MONCALL                     , std::string("IC_MONCALL                     ")},
    {IC_SET_REALTIME_ISO_0          , std::string("IC_SET_REALTIME_ISO_0          ")},
    {IC_SET_REALTIME_ISO_1          , std::string("IC_SET_REALTIME_ISO_1          ")},
    {IC_EXIT_MCELL_TEST             , std::string("IC_EXIT_MCELL_TEST             ")}
};

void Intercom::send(int message, int length, int param) {
    ic_message_map::iterator it = ic_message.find(message);
    if (it != ic_message.end()) {
        std::cout << "Send to intercom: " << it->second << "::" << length << "::" << std::hex << param << std::dec << std::endl;
    }
    else {
        std::cout << "Send to intercom: " << message << "::" << length << "::" << std::hex << param << std::dec << std::endl;
    }
    if (message == IC_SET_AF_POINT) {
        DPData.af_point = param;
    }
}

EOS420D::EOS420D() {
    GUIMode = GUIMODE_OFF;
    DPData.af_point = AF_PATTERN_CENTER;
    DPData.iso = ISO_100;
}

void EOS420D::display_af_patterns(void) {
    std::string dsp[5] = {
        std::string("     "),
        std::string("     "),
        std::string("     "),
        std::string("     "),
        std::string("     ")
    };
    if (DPData.af_point & AF_POINT_C)
        dsp[2][2] = 'x';
    if (DPData.af_point &AF_POINT_T)
        dsp[0][2] = 'x';
    if (DPData.af_point & AF_POINT_B)
        dsp[4][2]  = 'x';
    if (DPData.af_point & AF_POINT_TL)
        dsp[1][1] = 'x';
    if (DPData.af_point & AF_POINT_TR)
        dsp[1][3] = 'x';
    if (DPData.af_point & AF_POINT_BL)
        dsp[3][1] = 'x';
    if (DPData.af_point & AF_POINT_BR)
        dsp[3][3] = 'x';
    if (DPData.af_point & AF_POINT_L)
        dsp[2][0] = 'x';
    if (DPData.af_point & AF_POINT_R)
        dsp[2][4] = 'x';
    for (int line = 1; line <= 5; line++) {
        display.print_line(line, dsp[line - 1].c_str());
    }
    display.redraw();
}

void EOS420D::post_message(action_t pMessage) {
    if (pMessage != NULL) {
        if (message_queue.size() == 0) {
            message_queue.push(pMessage);
            pMessage();
            message_queue.pop();
            // Empty queue that could have been stored by pMessage
            while (!message_queue.empty()) {
                action_t current_action = message_queue.front();
                current_action();
                message_queue.pop();
            }
        }
        else {
            message_queue.push(pMessage);
        }
    }
}

std::map<button_t, guimode_t> switch_guimode = {
    {BUTTON_ZOOM_OUT, GUIMODE_AFPATTERN},
    {BUTTON_DOWN, GUIMODE_WB}
};

void EOS420D::hw_control(const Button button) {
}

void EOS420D::button_received(KeypadInput ki) {
    if (ki.button == BUTTON_NONE) {
        hw_control(ki.key);
    }
    else {
        button_handler(ki.button, TRUE);
    }
    // Set GUIMODE according to button
    if (status.menu_running)
        GUIMode = GUIMODE_420D;
    if (GUIMode == GUIMODE_OFF) {
        std::map<button_t, guimode_t>::iterator it = switch_guimode.find(ki.button);
        if (it != switch_guimode.end()) {
            GUIMode = it->second;
            FaceStatus = 0;
        }
    }
    else if (ki.button == BUTTON_RELEASE) {
        unsigned char msg[] = {(unsigned char)1, (unsigned char)IC_DIALOGOFF};
        intercom_proxy(0, msg);
        if (!status.menu_running) {
            display.clear();
            display.redraw();
        }
        GUIMode = GUIMODE_OFF;
    }
    if (GUIMode == GUIMODE_AFPATTERN)
        display_af_patterns();
}


#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void GetLanguageStr(int lang_id, char * lang_str) {
    strcpy(lang_str, "ENGLISH");
}

#ifdef __cplusplus
}
#endif // __cplusplus

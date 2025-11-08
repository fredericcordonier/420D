/**
 * \file debug.c
 * \brief Debug functions, disabled.
 */
#include <clock.h>
#include <ioLib.h>
#include <time.h>
#include <vxworks.h>

#include "firmware/eventproc.h"
#include "firmware/fio.h"
#include "firmware/gui.h"

#include "settings.h"

#include "debug.h"

#ifdef ENABLE_DEBUG

const char *debug_mc_name(int event) {
    static char name[20];

    switch (event) {
    case GUI_GOT_TOP_OF_CONTROL:
        return "GUI_GOT_TOP_OF_CONTROL";
    case MC_INFOANDCTRL:
        return "MC_INFOANDCTRL";
    case MC_BUTTON:
        return "MC_BUTTON";
    case MC_ACTION:
        return "MC_ACTION";
    case MC_DISPLAY_MODE:
        return "MC_DISPLAY_MODE";
    case MC_START_MODE:
        return "MC_START_MODE";
    case MC_DriveNotifyCallBack:
        return "MC_DriveNotifyCallBack";
    case MC_ChangeAvailShot:
        return "MC_ChangeAvailShot";
    case MC_ShutDown:
        return "MC_ShutDown";
    case MC_ChangeNotifyCallback:
        return "MC_ChangeNotifyCallback";
    case MC_CardDoor_Emergency:
        return "MC_CardDoor_Emergency";
    case MC_REQ_UI_LOCK:
        return "MC_REQ_UI_LOCK";
    case MC_REQ_UI_UNLOCK:
        return "MC_REQ_UI_UNLOCK";
    default:
        sprintf(name, "0x%08X", event);
        return name;
    }
}

const char *debug_btn_name(int btn) {
    static char name[20];

    switch (btn) {
    case MC_BUTTON_MENU:
        return "MC_BUTTON_MENU";
    case MC_BUTTON_DISP:
        return "MC_BUTTON_DISP";
    case MC_BUTTON_JUMP:
        return "MC_BUTTON_JUMP";
    case MC_BUTTON_PLAY:
        return "MC_BUTTON_PLAY";
    case MC_BUTTON_TRASH:
        return "MC_BUTTON_TRASH";
    case MC_BUTTON_SET:
        return "MC_BUTTON_SET";
    case MC_DIALOG1:
        return "MC_DIALOG1";
    case MC_DIALOG2:
        return "MC_DIALOG2";
    case MC_BUTTON_WHEEL:
        return "MC_BUTTON_WHEEL";
    case MC_BUTTON_WHEEL1:
        return "MC_BUTTON_WHEEL1";
    case MC_BUTTON_WHEEL_LEFT:
        return "MC_BUTTON_WHEEL_LEFT";
    case MC_BUTTON_WHEEL_RIGHT:
        return "MC_BUTTON_WHEEL_RIGHT";
    case MC_BUTTON_CARD_DOOR_OPEN:
        return "MC_BUTTON_CARD_DOOR_OPEN";
    case MC_BUTTON_UNK1:
        return "MC_BUTTON_UNK1";
    case MC_BUTTON_POWER:
        return "MC_BUTTON_POWER";
    case MC_BUTTON_BATTERY_DOOR_OPEN:
        return "MC_BUTTON_BATTERY_DOOR_OPEN";
    case MC_BUTTON_UP:
        return "MC_BUTTON_UP";
    case MC_BUTTON_DOWN:
        return "MC_BUTTON_DOWN";
    case MC_BUTTON_RIGHT:
        return "MC_BUTTON_RIGHT";
    case MC_BUTTON_LEFT:
        return "MC_BUTTON_LEFT";
    case MC_BUTTON_HALF_SHUTTER:
        return "MC_BUTTON_HALF_SHUTTER";
    case MC_BUTTON_FULL_SHUTTER:
        return "MC_BUTTON_FULL_SHUTTER";
    case MC_BUTTON_DP:
        return "MC_BUTTON_DP";
    case MC_AFPDLGON:
        return "MC_AFPDLGON";
    case MC_BUTTON_DRIVE:
        return "MC_BUTTON_DRIVE";
    case MC_BUTTON_AV:
        return "MC_BUTTON_AV";
    case MC_BUTTON_UNK2:
        return "MC_BUTTON_UNK2";
    default:
        sprintf(name, "0x%08X", btn);
        return name;
    }
}

const char *debug_gui_name(int event) {
    static char name[20];

    switch (event) {
    case GUI_GOT_TOP_OF_CONTROL:
        return "GUI_GOT_TOP_OF_CONTROL";
    case GUI_LOST_TOP_OF_CONTROL:
        return "GUI_LOST_TOP_OF_CONTROL";
    case GUI_INITIALIZE_CONTROLLER:
        return "GUI_INITIALIZE_CONTROLLER";
    case GUI_TERMINATE_WINSYS:
        return "GUI_TERMINATE_WINSYS";
    case GUI_DELETE_DIALOG_REQUEST:
        return "GUI_DELETE_DIALOG_REQUEST";
    case GUI_BUTTON_RIGHT:
        return "GUI_BUTTON_RIGHT";
    case GUI_BUTTON_LEFT:
        return "GUI_BUTTON_LEFT";
    case GUI_BUTTON_UP:
        return "GUI_BUTTON_UP";
    case GUI_BUTTON_DOWN:
        return "GUI_BUTTON_DOWN";
    case GUI_BUTTON_MENU:
        return "GUI_BUTTON_MENU";
    case GUI_BUTTON_JUMP:
        return "GUI_BUTTON_JUMP";
    case GUI_BUTTON_SET:
        return "GUI_BUTTON_SET";
    case GUI_BUTTON_ZOOM_IN_PRESS:
        return "GUI_BUTTON_ZOOM_IN_PRESS";
    case GUI_BUTTON_ZOOM_IN_RELEASE:
        return "GUI_BUTTON_ZOOM_IN_RELEASE";
    case GUI_BUTTON_ZOOM_OUT_PRESS:
        return "GUI_BUTTON_ZOOM_OUT_PRESS";
    case GUI_BUTTON_ZOOM_OUT_RELEASE:
        return "GUI_BUTTON_ZOOM_OUT_RELEASE";
    case GUI_BUTTON_DISP:
        return "GUI_BUTTON_DISP";
    case GUI_BUTTON_DIAL_RIGHT:
        return "GUI_BUTTON_DIAL_RIGHT";
    case GUI_BUTTON_DIAL_LEFT:
        return "GUI_BUTTON_DIAL_LEFT";
    case GUI_BUTTON_PLAY:
        return "GUI_BUTTON_PLAY";
    case GUI_BUTTON_TRASH:
        return "GUI_BUTTON_TRASH";
    case GUI_BUTTON_DP:
        return "GUI_BUTTON_DP";
    case GUI_BUTTON_CF_CARD:
        return "GUI_BUTTON_CF_CARD";
    case GUI_START_MENU_MODE:
        return "GUI_START_MENU_MODE";
    case GUI_START_INFO_MODE:
        return "GUI_START_INFO_MODE";
    case GUI_START_PLAY_MODE:
        return "GUI_START_PLAY_MODE";
    case GUI_START_QR_MODE:
        return "GUI_START_QR_MODE";
    case GUI_START_NOCFWARNING_MODE:
        return "GUI_START_NOCFWARNING_MODE";
    case GUI_START_FULLNOWARNING_MODE:
        return "GUI_START_FULLNOWARNING_MODE";
    case GUI_START_FULLCF:
        return "GUI_START_FULLCF";
    case GUI_START_ERRCF_ERROR:
        return "GUI_START_ERRCF_ERROR";
    case GUI_START_ERRCF_FULLNO:
        return "GUI_START_ERRCF_FULLNO";
    case GUI_START_ERRCF_PROTECT:
        return "GUI_START_ERRCF_PROTECT";
    case GUI_START_ERRORCODE_MODE:
        return "GUI_START_ERRORCODE_MODE";
    case GUI_START_INCOMPIMAGE_MODE:
        return "GUI_START_INCOMPIMAGE_MODE";
    case GUI_START_RTCSET_MODE:
        return "GUI_START_RTCSET_MODE";
    case GUI_START_IDLE_MODE:
        return "GUI_START_IDLE_MODE";
    case GUI_POST_QR_IMAGE:
        return "GUI_POST_QR_IMAGE";
    case GUI_DISP_RTCSET_MODE:
        return "GUI_DISP_RTCSET_MODE";
    case GUI_DISP_NOCFWARNING_MODE:
        return "GUI_DISP_NOCFWARNING_MODE";
    case GUI_DISP_FULLNOWARNING_MODE:
        return "GUI_DISP_FULLNOWARNING_MODE";
    case GUI_DISP_FULLCF:
        return "GUI_DISP_FULLCF";
    case GUI_DISP_ERRCF_ERROR:
        return "GUI_DISP_ERRCF_ERROR";
    case GUI_DISP_ERRCF_FULLNO:
        return "GUI_DISP_ERRCF_FULLNO";
    case GUI_DISP_ERRCF_PROTECT:
        return "GUI_DISP_ERRCF_PROTECT";
    case GUI_DISP_ERRORCODE_MODE:
        return "GUI_DISP_ERRORCODE_MODE";
    case GUI_DISP_INCOMPIMAGE_MODE:
        return "GUI_DISP_INCOMPIMAGE_MODE";
    case GUI_FINISH_QR_MODE:
        return "GUI_FINISH_QR_MODE";
    case GUI_DISP_USBCONNECTTYPE_MODE:
        return "GUI_DISP_USBCONNECTTYPE_MODE";
    case GUI_DISP_DIRECTTRANSFER_MODE:
        return "GUI_DISP_DIRECTTRANSFER_MODE";
    case GUI_START_USBCONNECTTYPE_MODE:
        return "GUI_START_USBCONNECTTYPE_MODE";
    case GUI_START_DIRECTTRANSFER_MODE:
        return "GUI_START_DIRECTTRANSFER_MODE";
    case GUI_DISP_OLC_MODE:
        return "GUI_DISP_OLC_MODE";
    case GUI_DISP_BULBCOUNT_MODE:
        return "GUI_DISP_BULBCOUNT_MODE";
    case GUI_START_OLC_MODE:
        return "GUI_START_OLC_MODE";
    case GUI_START_BULBCOUNT_MODE:
        return "GUI_START_BULBCOUNT_MODE";
    case GUI_DISP_ISO_MODE:
        return "GUI_DISP_ISO_MODE";
    case GUI_DISP_AF_MODE:
        return "GUI_DISP_AF_MODE";
    case GUI_DISP_WB_MODE:
        return "GUI_DISP_WB_MODE";
    case GUI_DISP_MES_MODE:
        return "GUI_DISP_MES_MODE";
    case GUI_BUTTON_DRIVE:
        return "GUI_BUTTON_DRIVE";
    case GUI_DISP_FP_SEL:
        return "GUI_DISP_FP_SEL";
    case GUI_DISP_QUAL_MODE:
        return "GUI_DISP_QUAL_MODE";
    case GUI_DISP_PICTURESTYLE_MODE:
        return "GUI_DISP_PICTURESTYLE_MODE";
    case GUI_DISP_FECOMP_MODE:
        return "GUI_DISP_FECOMP_MODE";
    case GUI_DISP_ACTIVESWEEP:
        return "GUI_DISP_ACTIVESWEEP";
    case GUI_DISP_ACTIVESWEEP_OLC:
        return "GUI_DISP_ACTIVESWEEP_OLC";
    case GUI_DISP_BATTEMPTY:
        return "GUI_DISP_BATTEMPTY";
    case GUI_START_ISO_MODE:
        return "GUI_START_ISO_MODE";
    case GUI_START_AF_MODE:
        return "GUI_START_AF_MODE";
    case GUI_START_WB_MODE:
        return "GUI_START_WB_MODE";
    case GUI_START_MES_MODE:
        return "GUI_START_MES_MODE";
    case GUI_START_DRIVE_MODE:
        return "GUI_START_DRIVE_MODE";
    case GUI_START_FP_SEL:
        return "GUI_START_FP_SEL";
    case GUI_START_QUAL_MODE:
        return "GUI_START_QUAL_MODE";
    case GUI_START_PICTURESTYLE_MODE:
        return "GUI_START_PICTURESTYLE_MODE";
    case GUI_START_FECOMP_MODE:
        return "GUI_START_FECOMP_MODE";
    case GUI_START_ACTIVESWEEP:
        return "GUI_START_ACTIVESWEEP";
    case GUI_START_ACTIVESWEEP_OLC:
        return "GUI_START_ACTIVESWEEP_OLC";
    case GUI_START_BATTEMPTY:
        return "GUI_START_BATTEMPTY";
    case GUI_FINISH_ACTIVESWEEP:
        return "GUI_FINISH_ACTIVESWEEP";
    case GUI_START_COLORBAR_DIALOG:
        return "GUI_START_COLORBAR_DIALOG";
    case GUI_START_LCDADJUST_DIALOG:
        return "GUI_START_LCDADJUST_DIALOG";
    case GUI_DELETE_COLORBAR_DIALOG:
        return "GUI_DELETE_COLORBAR_DIALOG";
    case GUI_DELETE_LCDADJUST_DIALOG:
        return "GUI_DELETE_LCDADJUST_DIALOG";
    case GUI_UNKNOWN1:
        return "GUI_UNKNOWN1";
    case GUI_UNKNOWN2:
        return "GUI_UNKNOWN2";

    default:
        sprintf(name, "0x%08X", event);
        return name;
    }
}

#endif // ENABLE_DEBUG

#ifdef ENABLE_DEBUG // _DPR_DATA

void dump_dpr_data(void) {
    debug_log("\nDUMPING DPR_DATA\n");
    debug_log("\t%s : 0x%08X [%d]", "ae", DPData.ae, DPData.ae);
    debug_log("\t%s : 0x%08X [%d]", "metering", DPData.metering,
              DPData.metering);
    debug_log("\t%s : 0x%08X [%d]", "efcomp", DPData.efcomp, DPData.efcomp);
    debug_log("\t%s : 0x%08X [%d]", "drive", DPData.drive, DPData.drive);
    debug_log("\t%s : 0x%08X [%d]", "wb", DPData.wb, DPData.wb);
    debug_log("\t%s : 0x%08X [%d]", "af", DPData.af, DPData.af);
    debug_log("\t%s : 0x%08X [%d]", "af_point", DPData.af_point,
              DPData.af_point);
    debug_log("\t%s : 0x%08X [%d]", "tv_val", DPData.tv_val, DPData.tv_val);
    debug_log("\t%s : 0x%08X [%d]", "av_val", DPData.av_val, DPData.av_val);
    debug_log("\t%s : 0x%08X [%d]", "av_comp", DPData.av_comp,
              DPData.av_comp);
    debug_log("\t%s : 0x%08X [%d]", "iso", DPData.iso, DPData.iso);
    debug_log("\t%s : 0x%08X [%d]", "red_eye", DPData.red_eye,
              DPData.red_eye);
    debug_log("\t%s : 0x%08X [%d]", "ae_bkt", DPData.ae_bkt, DPData.ae_bkt);
    debug_log("\t%s : 0x%08X [%d]", "wb_bkt", DPData.wb_bkt, DPData.wb_bkt);
    debug_log("\t%s : 0x%08X [%d]", "beep", DPData.beep, DPData.beep);
    debug_log("\t%s : 0x%08X [%d]", "color_temp", DPData.color_temp,
              DPData.color_temp);
    debug_log("\t%s : 0x%08X [%d]", "auto_power_off", DPData.auto_power_off,
              DPData.auto_power_off);
    debug_log("\t%s : 0x%08X [%d]", "view_type", DPData.view_type,
              DPData.view_type);
    debug_log("\t%s : 0x%08X [%d]", "review_time", DPData.review_time,
              DPData.review_time);
    debug_log("\t%s : 0x%08X [%d]", "auto_rotate", DPData.auto_rotate,
              DPData.auto_rotate);
    debug_log("\t%s : 0x%08X [%d]", "lcd_brightness", DPData.lcd_brightness,
              DPData.lcd_brightness);
    debug_log("\t%s : 0x%08X [%d]", "date_time", DPData.date_time,
              DPData.date_time);
    debug_log("\t%s : 0x%08X [%d]", "file_numbering", DPData.file_numbering,
              DPData.file_numbering);
    debug_log("\t%s : 0x%08X [%d]", "language", DPData.language,
              DPData.language);
    debug_log("\t%s : 0x%08X [%d]", "video_system", DPData.video_system,
              DPData.video_system);
    debug_log("\t%s : 0x%08X [%d]", "picture_style_mode",
              DPData.picture_style_mode, DPData.picture_style_mode);
    debug_log("\t%s : 0x%08X [%d]", "histogram", DPData.histogram,
              DPData.histogram);
    debug_log("\t%s : 0x%08X [%d]", "disp_afpoint", DPData.disp_afpoint,
              DPData.disp_afpoint);
    debug_log("\t%s : 0x%08X [%d]", "color_space", DPData.color_space,
              DPData.color_space);
    debug_log("\t%s : 0x%08X [%d]", "img_format", DPData.img_format,
              DPData.img_format);
    debug_log("\t%s : 0x%08X [%d]", "img_size", DPData.img_size,
              DPData.img_size);
    debug_log("\t%s : 0x%08X [%d]", "img_quality", DPData.img_quality,
              DPData.img_quality);
    debug_log("\t%s : 0x%08X [%d]", "cfmenupos", DPData.cfmenupos,
              DPData.cfmenupos);
    debug_log("\t%s : 0x%08X [%d]", "menupos", DPData.menupos,
              DPData.menupos);
    debug_log("\t%s : 0x%08X [%d]", "wbcomp_gm", DPData.wbcomp_gm,
              DPData.wbcomp_gm);
    debug_log("\t%s : 0x%08X [%d]", "wbcomp_ab", DPData.wbcomp_ab,
              DPData.wbcomp_ab);
    debug_log("\t%s : 0x%08X [%d]", "forbid_rel", DPData.forbid_rel,
              DPData.forbid_rel);
    debug_log("\t%s : 0x%08X [%d]", "cf_set_button_func",
              DPData.cf_set_button_func, DPData.cf_set_button_func);
    debug_log("\t%s : 0x%08X [%d]", "cf_nr_for_long_exposure",
              DPData.cf_nr_for_long_exposure, DPData.cf_nr_for_long_exposure);
    debug_log("\t%s : 0x%08X [%d]", "cf_efav_fix_x", DPData.cf_efav_fix_x,
              DPData.cf_efav_fix_x);
    debug_log("\t%s : 0x%08X [%d]", "cf_afael_active_button",
              DPData.cf_afael_active_button, DPData.cf_afael_active_button);
    debug_log("\t%s : 0x%08X [%d]", "cf_emit_aux", DPData.cf_emit_aux,
              DPData.cf_emit_aux);
    debug_log("\t%s : 0x%08X [%d]", "cf_explevel_inc_third",
              DPData.cf_explevel_inc_third, DPData.cf_explevel_inc_third);
    debug_log("\t%s : 0x%08X [%d]", "cf_emit_flash", DPData.cf_emit_flash,
              DPData.cf_emit_flash);
    debug_log("\t%s : 0x%08X [%d]", "cf_extend_iso", DPData.cf_extend_iso,
              DPData.cf_extend_iso);
    debug_log("\t%s : 0x%08X [%d]", "cf_aeb_sequence",
              DPData.cf_aeb_sequence, DPData.cf_aeb_sequence);
    debug_log("\t%s : 0x%08X [%d]", "cf_si_indicate", DPData.cf_si_indicate,
              DPData.cf_si_indicate);
    debug_log("\t%s : 0x%08X [%d]", "cf_menu_pos", DPData.cf_menu_pos,
              DPData.cf_menu_pos);
    debug_log("\t%s : 0x%08X [%d]", "cf_mirror_up_lock",
              DPData.cf_mirror_up_lock, DPData.cf_mirror_up_lock);
    debug_log("\t%s : 0x%08X [%d]", "cf_fpsel_method",
              DPData.cf_fpsel_method, DPData.cf_fpsel_method);
    debug_log("\t%s : 0x%08X [%d]", "cf_flash_metering",
              DPData.cf_flash_metering, DPData.cf_flash_metering);
    debug_log("\t%s : 0x%08X [%d]", "cf_flash_sync_rear",
              DPData.cf_flash_sync_rear, DPData.cf_flash_sync_rear);
    debug_log("\t%s : 0x%08X [%d]", "cf_safety_shift",
              DPData.cf_safety_shift, DPData.cf_safety_shift);
    debug_log("\t%s : 0x%08X [%d]", "cf_lens_button", DPData.cf_lens_button,
              DPData.cf_lens_button);
    debug_log("\t%s : 0x%08X [%d]", "cf_original_eval",
              DPData.cf_original_eval, DPData.cf_original_eval);
    debug_log("\t%s : 0x%08X [%d]", "cf_qr_magnify", DPData.cf_qr_magnify,
              DPData.cf_qr_magnify);
    debug_log("\t%s : 0x%08X [%d]", "batt_bclevel", DPData.batt_bclevel,
              DPData.batt_bclevel);
    debug_log("\t%s : 0x%08X [%d]", "batt_type", DPData.batt_type,
              DPData.batt_type);
    debug_log("\t%s : 0x%08X [%d]", "print_user_property",
              DPData.print_user_property, DPData.print_user_property);
    debug_log("\t%s : 0x%08X [%d]", "printer_property",
              DPData.printer_property, DPData.printer_property);
    debug_log("\t%s : 0x%08X [%d]", "cluster_size", DPData.cluster_size,
              DPData.cluster_size);
    debug_log("\t%s : 0x%08X [%d]", "remain_cluster", DPData.remain_cluster,
              DPData.remain_cluster);
    debug_log("\t%s : 0x%08X [%d]", "file_id", DPData.file_id,
              DPData.file_id);
    debug_log("\t%s : 0x%08X [%d]", "avo", DPData.avo, DPData.avo);
    debug_log("\t%s : 0x%08X [%d]", "avmax", DPData.avmax, DPData.avmax);
    debug_log("\t%s : 0x%08X [%d]", "ef_lens_exist", DPData.ef_lens_exist,
              DPData.ef_lens_exist);
    debug_log("\t%s : 0x%08X [%d]", "avail_shot", DPData.avail_shot,
              DPData.avail_shot);
    debug_log("\t%s : 0x%08X [%d]", "qrev_imgprop", DPData.qrev_imgprop,
              DPData.qrev_imgprop);
    debug_log("\t%s : 0x%08X [%d]", "printer_connect_type",
              DPData.printer_connect_type, DPData.printer_connect_type);
    debug_log("\t%s : 0x%08X [%d]", "mwbdat_red", DPData.mwbdat_red,
              DPData.mwbdat_red);
    debug_log("\t%s : 0x%08X [%d]", "mwbdat_green1", DPData.mwbdat_green1,
              DPData.mwbdat_green1);
    debug_log("\t%s : 0x%08X [%d]", "mwbdat_green2", DPData.mwbdat_green2,
              DPData.mwbdat_green2);
    debug_log("\t%s : 0x%08X [%d]", "mwbdat_blue", DPData.mwbdat_blue,
              DPData.mwbdat_blue);
    debug_log("\t%s : 0x%08X [%d]", "videojack_connect",
              DPData.videojack_connect, DPData.videojack_connect);
    debug_log("\t%s : 0x%08X [%d]", "incomp_image", DPData.incomp_image,
              DPData.incomp_image);
    debug_log("\t%s : 0x%08X [%d]", "status_busy_flag",
              DPData.status_busy_flag, DPData.status_busy_flag);
    debug_log("\t%s : 0x%08X [%d]", "special_bc_result",
              DPData.special_bc_result, DPData.special_bc_result);
    debug_log("\t%s : 0x%08X [%d]", "reset_offtimer", DPData.reset_offtimer,
              DPData.reset_offtimer);
    debug_log("\t%s : 0x%08X [%d]", "printer_device_property",
              DPData.printer_device_property, DPData.printer_device_property);
    debug_log("\t%s : 0x%08X [%d]", "printer_job_property",
              DPData.printer_job_property, DPData.printer_job_property);
    debug_log("\t%s : 0x%08X [%d]", "dpof_setting", DPData.dpof_setting,
              DPData.dpof_setting);
    debug_log("\t%s : 0x%08X [%d]", "gui_mode", DPData.gui_mode,
              DPData.gui_mode);
    debug_log("\t%s : 0x%08X [%d]", "dp_request_return",
              DPData.dp_request_return, DPData.dp_request_return);
    debug_log("\t%s : 0x%08X [%d]", "commadp_chg_config",
              DPData.commadp_chg_config, DPData.commadp_chg_config);
    debug_log("\t%s : 0x%08X [%d]", "play_jump_type", DPData.play_jump_type,
              DPData.play_jump_type);
    debug_log("\t%s : 0x%08X [%d]", "capability_setting",
              DPData.capability_setting, DPData.capability_setting);
    debug_log("\t%s : 0x%08X [%d]", "selected_af_mode",
              DPData.selected_af_mode, DPData.selected_af_mode);
    debug_log("\t%s : 0x%08X [%d]", "disable_ui_lock",
              DPData.disable_ui_lock, DPData.disable_ui_lock);
    debug_log("\t%s : 0x%08X [%d]", "isd_style_flag", DPData.isd_style_flag,
              DPData.isd_style_flag);
    debug_log("\t%s : 0x%08X [%d]", "ptp", DPData.ptp, DPData.ptp);
    debug_log("\t%s : 0x%08X [%d]", "playback_file_id",
              DPData.playback_file_id, DPData.playback_file_id);
    debug_log("\t%s : 0x%08X [%d]", "playback_file_id_pos",
              DPData.playback_file_id_pos, DPData.playback_file_id_pos);
    debug_log("\t%s : 0x%08X [%d]", "usb_connect_type",
              DPData.usb_connect_type, DPData.usb_connect_type);
    debug_log("\t%s : 0x%08X [%d]", "face_sensor_disable",
              DPData.face_sensor_disable, DPData.face_sensor_disable);
    debug_log("\t%s : 0x%08X [%d]", "field_178", DPData.field_178,
              DPData.field_178);
    debug_log("\t%s : 0x%08X [%d]", "field_17C", DPData.field_17C,
              DPData.field_17C);
    debug_log("\t%s : 0x%08X [%d]", "field_180", DPData.field_180,
              DPData.field_180);
    debug_log("\t%s : 0x%08X [%d]", "field_184", DPData.field_184,
              DPData.field_184);
    debug_log("\t%s : 0x%08X [%d]", "cf_tft_on_power_on",
              DPData.cf_tft_on_power_on, DPData.cf_tft_on_power_on);
    debug_log("\t%s : 0x%08X [%d]", "field_18C", DPData.field_18C,
              DPData.field_18C);
    debug_log("\t%s : 0x%08X [%d]", "field_190", DPData.field_190,
              DPData.field_190);

    debug_log("\nEND\n");
}

#endif // ENABLE_DEBUG_DPR_DATA

#ifdef MEM_DUMP

void dump_memory() {
    char filename[20] = "A:/12345678.MEM";
    time_t t;
    struct tm tm;

    time(&t);
    localtime_r(&t, &tm);

    sprintf(filename, "A:/%02d%02d%02d%02d.MEM", tm.tm_mday, tm.tm_hour,
            tm.tm_min, tm.tm_sec);

    debug_log("Dumping memory to %s.\n", filename);
    int file = FIO_OpenFile(filename, O_CREAT | O_WRONLY, 644);

    if (file == -1) {
        debug_log("ERROR: can't open file for writing (%s)", filename);
        beep();
        beep();
    } else {
        int addr = 0;
        int power_off_state = DPData.auto_power_off;

        send_to_intercom(IC_SET_AUTO_POWER_OFF, FALSE);

        while (addr < 0x800000) { // dump 8MB of RAM
            char buf[0x800];
            // i don't know why, but if we try to pass the mem address (addr)
            // directly to FIO_WriteFile, we get zero-filled file... so we need
            // local buffer as a proxy note: do not increase the size of the
            // local buffer too much, because it is in the stack
            LEDBLUE ^= 2;
            memcpy(buf, (void *)addr, 0x800);
            FIO_WriteFile(file, buf, 0x800);
            addr += 0x800;
        }
        FIO_CloseFile(file);

        send_to_intercom(IC_SET_AUTO_POWER_OFF, power_off_state);
    }
    beep();
}

static void mem_dumper_task() {
    int i;

    beep();

    for (i = 0; i < 10; i++) {
        LEDBLUE ^= 2;
        SleepTask(500);
    }

    dump_memory();
}

void dump_memory_after_5s() {
    CreateTask("memdumper", 0x1e, 0x1000, mem_dumper_task, 0);
}

#endif // MEM_DUMP

void dump_log() {
    debug_log("Dumping the log.\n");
    dumpf();
    beep();
}

void print_info() {
    // print some info to the log
    eventproc_RiseEvent("about");

    // print last errors to the log
    eventproc_RiseEvent("PrintICUInfo");

    // print current DP settings to the log
    eventproc_RiseEvent("PrintDPStatus");

    beep();
    debug_log("Info dumped.\n");
}

void start_debug_mode() {
    int file;
    char filename[20] = "A:/DEBUG.LOG";
    const char separator[] = "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-\n";

    time_t t;
    struct tm tm;

    time(&t);
    localtime_r(&t, &tm);

    if (settings.logfile_mode == LOGFILE_MODE_NEW)
        sprintf(filename, "A:/%02d%02d%02d%02d.LOG", tm.tm_mon + 1, tm.tm_mday,
                tm.tm_hour, tm.tm_min);

    // O_APPEND is not working in VxWorks, so we seek to the end later
    if ((file = FIO_OpenFile(filename, O_CREAT | O_WRONLY)) != -1) {
        if (settings.logfile_mode == LOGFILE_MODE_APPEND)
            FIO_SeekFile(file, 0, 2 /*SEEK_END*/);

        // redirect stdout and stderr to our file
        ioGlobalStdSet(1, file);
        ioGlobalStdSet(2, file);
    }

    printf(separator);
    printf("::::: %04d-%02d-%02d %02d:%02d:%02d :::::\n", tm.tm_year + 1900,
           tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    printf(separator);
    printf("\n");

    beep();
}

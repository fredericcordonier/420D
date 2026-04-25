#pragma once

#include "exposure.h"

typedef enum {
    FV_MODE_TV_AUTO = 0xFF,
    FV_MODE_AV_AUTO = 0xFF
} fv_mode_values;

typedef enum {
    FV_MODE_P = 'P',
    FV_MODE_A = 'A',
    FV_MODE_T = 'T',
    FV_MODE_M = 'M'
} fv_mode;

void fv_mode_start_shortcut(void);
void fv_mode_apply(void);
void fv_mode_set_av(av_t av);
void fv_mode_set_tv(tv_t tv);

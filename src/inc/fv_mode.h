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

void fv_mode_init(void);
void fv_mode_apply(void);

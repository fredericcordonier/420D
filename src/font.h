/**
 * \file font.h
 * \brief Header file for font management.
 */
#ifndef _font_h_
#define _font_h_

#include <stdint.h>
#include <vxworks.h>

typedef struct {
    unsigned width;
    unsigned height;
    uint16_t bitmap[];
} font_t;

#ifdef USE_FONT_SMALL
extern font_t font_small;
#endif
#ifdef USE_FONT_MED
extern font_t font_med;
#endif
#ifdef USE_FONT_LARGE
extern font_t font_large;
#endif

#endif

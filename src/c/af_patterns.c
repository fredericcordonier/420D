#include <vxworks.h>
#include "settings.h"

/**
 * @file af_patterns.c
 * @brief Management of AF patterns.
 *
 */

/**
 * @defgroup af_pattern Management of AF patterns.
 * @brief Management of AF patterns.
 *
 * 400plus offers a range of additional AF-Pattern selections, instead of just
 * the two (Manual AF point selection, and Automatic AF point selection) which
 * are included standard in the camera. These additional AF-Pattern selections
 * provide a wide range of multi-point selection patterns, using different
 * combinations of the 9 focus points, for some creative AF capabilities.
 *
 * To use the additional AF-Pattern selections during shooting:
 * - press the ZOOM IN button to enter the standard "AF-Point Selection" dialog
 * - press the ZOOM IN button again to enter the new "AF-Pattern Selection"
 * dialog
 * - use the UP, DOWN, LEFT, RIGHT, and SET buttons there to move around the
 * different AF-Patterns available.
 * - half-press the shutter-release button to exit the dialog.
 *
 * The following patterns are available:
 *
 * 1. Selected with RIGHT / LEFT buttons
 * ~~~~
 * |--------------|--------------|--------------|--------------|--------------|--------------|--------------|
 * |      -       |      -       |      x       |      x       |      x       |
 * -       |      -       | |    x   -     |    x   -     |    x   -     |    -
 * -     |    -   x     |    -   x     |    -   x     | |  x   -   -   |  x   x
 * -   |  x   x   -   |  -   x   -   |  -   x   x   |  -   x   x   |  -   -   x
 * | |    x   -     |    x   -     |    x   -     |    -   -     |    -   x | -
 * x     |    -   x     | |      -       |      -       |      x       |      x
 * |      x       |      -       |      -       |
 * |--------------|--------------|--------------|--------------|--------------|--------------|--------------|
 * ~~~~
 * 2. Selected with DOWN / UP buttons
 * ~~~~
 * |--------------|--------------|--------------|--------------|--------------|--------------|--------------|
 * |      x       |      x       |      x       |      -       |      -       |
 * -       |      -       | |    x   x     |    x   x     |    x   x     |    -
 * -     |    -   -     |    -   -     |    -   -     | |  -   -   -   |  -   x
 * -   |  x   x   x   |  x   x   x   |  x   x   x   |  -   x   -   |  -   -   -
 * | |    -   -     |    -   -     |    -   -     |    -   -     |    x   x | x
 * x     |    x   x     | |      -       |      -       |      -       |      -
 * |      x       |      x       |      x       |
 * |--------------|--------------|--------------|--------------|--------------|--------------|--------------|
 * ~~~~
 * 3. Selected with SET button (twice)
 * ~~~~
 * |--------------|
 * |      -       |
 * |    x   x     |
 * |  -   x   -   |
 * |    x   x     |
 * |      -       |
 * |--------------|
 * ~~~~
 */
#include "firmware.h"
#include "firmware/camera.h"

#include "macros.h"
#include "main.h"

#include "intercom.h"
#include "utils.h"

#include "af_patterns.h"

static void afp_set(int afp);

/**
 * @brief List of AF patterns
 * @ingroup af_patterns
 *
 * AF patterns are bitsets, 1 bit per point. The structures indicate the bitset,
 * as well as the following one in each "direction" (each pressed key).
 */
pattern_map_item_t pattern_map[] = {
    /*  pattern                    next center        next top next bottom next
       left                 next right            */
    {AF_PATTERN_CENTER, AF_PATTERN_SQUARE, AF_PATTERN_TOPHALF,
     AF_PATTERN_BOTTOMHALF, AF_PATTERN_LEFTHALF, AF_PATTERN_RIGHTHALF},
    {AF_PATTERN_SQUARE, AF_PATTERN_HLINE, AF_PATTERN_TOPHALF,
     AF_PATTERN_BOTTOMHALF, AF_PATTERN_LEFTHALF, AF_PATTERN_RIGHTHALF},

    {AF_PATTERN_TOP, AF_PATTERN_CENTER, AF_PATTERN_TOP, AF_PATTERN_TOPTRIANGLE,
     AF_PATTERN_TOPLEFT, AF_PATTERN_TOPRIGHT},
    {AF_PATTERN_TOPTRIANGLE, AF_PATTERN_CENTER, AF_PATTERN_TOP,
     AF_PATTERN_TOPDIAMOND, AF_PATTERN_LEFTTRIANGLE, AF_PATTERN_RIGHTTRIANGLE},
    {AF_PATTERN_TOPDIAMOND, AF_PATTERN_CENTER, AF_PATTERN_TOPTRIANGLE,
     AF_PATTERN_TOPHALF, AF_PATTERN_LEFTDIAMOND, AF_PATTERN_RIGHTDIAMOND},
    {AF_PATTERN_TOPHALF, AF_PATTERN_CENTER, AF_PATTERN_TOPDIAMOND,
     AF_PATTERN_HLINE, AF_PATTERN_LEFTHALF, AF_PATTERN_RIGHTHALF},

    {AF_PATTERN_BOTTOM, AF_PATTERN_CENTER, AF_PATTERN_BOTTOMTRIANGLE,
     AF_PATTERN_BOTTOM, AF_PATTERN_BOTTOMLEFT, AF_PATTERN_BOTTOMRIGHT},
    {AF_PATTERN_BOTTOMTRIANGLE, AF_PATTERN_CENTER, AF_PATTERN_BOTTOMDIAMOND,
     AF_PATTERN_BOTTOM, AF_PATTERN_LEFTTRIANGLE, AF_PATTERN_RIGHTTRIANGLE},
    {AF_PATTERN_BOTTOMDIAMOND, AF_PATTERN_CENTER, AF_PATTERN_BOTTOMHALF,
     AF_PATTERN_BOTTOMTRIANGLE, AF_PATTERN_LEFTDIAMOND,
     AF_PATTERN_RIGHTDIAMOND},
    {AF_PATTERN_BOTTOMHALF, AF_PATTERN_CENTER, AF_PATTERN_HLINE,
     AF_PATTERN_BOTTOMDIAMOND, AF_PATTERN_LEFTHALF, AF_PATTERN_RIGHTHALF},

    {AF_PATTERN_TOPLEFT, AF_PATTERN_CENTER, AF_PATTERN_TOP,
     AF_PATTERN_BOTTOMLEFT, AF_PATTERN_LEFT, AF_PATTERN_TOPRIGHT},
    {AF_PATTERN_TOPRIGHT, AF_PATTERN_CENTER, AF_PATTERN_TOP,
     AF_PATTERN_BOTTOMRIGHT, AF_PATTERN_TOPLEFT, AF_PATTERN_RIGHT},
    {AF_PATTERN_BOTTOMLEFT, AF_PATTERN_CENTER, AF_PATTERN_TOPLEFT,
     AF_PATTERN_BOTTOM, AF_PATTERN_LEFT, AF_PATTERN_BOTTOMRIGHT},
    {AF_PATTERN_BOTTOMRIGHT, AF_PATTERN_CENTER, AF_PATTERN_TOPRIGHT,
     AF_PATTERN_BOTTOM, AF_PATTERN_BOTTOMLEFT, AF_PATTERN_RIGHT},

    {AF_PATTERN_LEFT, AF_PATTERN_CENTER, AF_PATTERN_TOPLEFT,
     AF_PATTERN_BOTTOMLEFT, AF_PATTERN_LEFT, AF_PATTERN_LEFTTRIANGLE},
    {AF_PATTERN_LEFTTRIANGLE, AF_PATTERN_CENTER, AF_PATTERN_TOPTRIANGLE,
     AF_PATTERN_BOTTOMTRIANGLE, AF_PATTERN_LEFT, AF_PATTERN_LEFTDIAMOND},
    {AF_PATTERN_LEFTDIAMOND, AF_PATTERN_CENTER, AF_PATTERN_TOPDIAMOND,
     AF_PATTERN_BOTTOMDIAMOND, AF_PATTERN_LEFTTRIANGLE, AF_PATTERN_LEFTHALF},
    {AF_PATTERN_LEFTHALF, AF_PATTERN_CENTER, AF_PATTERN_TOPHALF,
     AF_PATTERN_BOTTOMHALF, AF_PATTERN_LEFTDIAMOND, AF_PATTERN_VLINE},

    {AF_PATTERN_RIGHT, AF_PATTERN_CENTER, AF_PATTERN_TOPRIGHT,
     AF_PATTERN_BOTTOMRIGHT, AF_PATTERN_RIGHTTRIANGLE, AF_PATTERN_RIGHT},
    {AF_PATTERN_RIGHTTRIANGLE, AF_PATTERN_CENTER, AF_PATTERN_TOPTRIANGLE,
     AF_PATTERN_BOTTOMTRIANGLE, AF_PATTERN_RIGHTDIAMOND, AF_PATTERN_RIGHT},
    {AF_PATTERN_RIGHTDIAMOND, AF_PATTERN_CENTER, AF_PATTERN_TOPDIAMOND,
     AF_PATTERN_BOTTOMDIAMOND, AF_PATTERN_RIGHTHALF, AF_PATTERN_RIGHTTRIANGLE},
    {AF_PATTERN_RIGHTHALF, AF_PATTERN_CENTER, AF_PATTERN_TOPHALF,
     AF_PATTERN_BOTTOMHALF, AF_PATTERN_VLINE, AF_PATTERN_RIGHTDIAMOND},

    {AF_PATTERN_HLINE, AF_PATTERN_VLINE, AF_PATTERN_TOPHALF,
     AF_PATTERN_BOTTOMHALF, AF_PATTERN_LEFTHALF, AF_PATTERN_RIGHTHALF},
    {AF_PATTERN_VLINE, AF_PATTERN_ALL, AF_PATTERN_TOPHALF,
     AF_PATTERN_BOTTOMHALF, AF_PATTERN_LEFTHALF, AF_PATTERN_RIGHTHALF},

    {AF_PATTERN_ALL, AF_PATTERN_CENTER, AF_PATTERN_TOPHALF,
     AF_PATTERN_BOTTOMHALF, AF_PATTERN_LEFTHALF, AF_PATTERN_RIGHTHALF},

    {AF_PATTERN_NONE}};

int afp_transformer(int pattern, direction_t direction);

/**
 * @brief set AFP selection and store it depending on orientation.
 */
static void afp_set(int afp) {
    send_to_intercom(IC_SET_AF_POINT, afp);
    switch (status.orientation) {
    case ORIENTATION_H:
        settings.af_pattern_horizontal = afp;
        break;
    case ORIENTATION_VL:
        settings.af_pattern_vertical_left = afp;
        break;
    case ORIENTATION_VR:
        settings.af_pattern_vertical_right = afp;
        break;
    }
}

/**
 * @brief Enter the AFP selection
 * @ingroup af_pattern
 *
 */
void afp_enter() { beep(); }

/**
 * @brief SET button in AFP selection
 * @ingroup af_pattern
 *
 */
void afp_center() {
    afp_set(afp_transformer(DPData.af_point, DIRECTION_CENTER));
}

/**
 * @brief Up button in AFP selection
 * @ingroup af_pattern
 *
 */
void afp_top() {
    afp_set(afp_transformer(DPData.af_point, DIRECTION_UP));}

/**
 * @brief Down button in AFP selection
 * @ingroup af_pattern
 *
 */
void afp_bottom() {
    afp_set(afp_transformer(DPData.af_point, DIRECTION_DOWN));
}

/**
 * @brief Left button in AFP selection
 * @ingroup af_pattern
 *
 */
void afp_left() {
    afp_set(afp_transformer(DPData.af_point, DIRECTION_LEFT));
}

/**
 * @brief Right button in AFP selection
 * @ingroup af_pattern
 *
 */
void afp_right() {
    afp_set(afp_transformer(DPData.af_point, DIRECTION_RIGHT));
}

/**
 * @brief Sets the next pattern according to direction
 * @ingroup af_pattern
 *
 * @param pattern Current pattern value
 * @param direction Direction to select next pattern
 * @return int The next pattern.
 */
int afp_transformer(int pattern, direction_t direction) {
    pattern_map_item_t *item;

    // Loop over all items in the pattern map
    for (item = pattern_map; item->pattern != AF_PATTERN_NONE; item++) {

        // When we find an item matching the current pattern...
        if (item->pattern == pattern) {

            // ... we return the next pattern, according to the direction
            // indicated
            switch (direction) {
            case DIRECTION_CENTER:
                return item->next_center;
            case DIRECTION_UP:
                return item->next_top;
            case DIRECTION_DOWN:
                return item->next_bottom;
            case DIRECTION_LEFT:
                return item->next_left;
            case DIRECTION_RIGHT:
                return item->next_right;
            }
        }
    }

    // Just in case something goes wrong
    return AF_PATTERN_CENTER;
}

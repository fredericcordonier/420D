#ifndef SERIALIZE_H
#define SERIALIZE_H

#include "settings.h"

#define SAVED_PARAM(s, x)                                                      \
    { C_##s##_##x##_TAG, (long)(&(((s *)NULL)->x)), (sizeof(((s *)NULL)->x)) }

int write_settings_file(int file, settings_t *px_settings);
int read_settings_file(int file, settings_t *px_settings);
int write_menu_order_file(int file, menu_order_t *px_menu_order);
int read_menu_order_file(int file, menu_order_t *px_menu_order);
#endif // SERIALIZE_H

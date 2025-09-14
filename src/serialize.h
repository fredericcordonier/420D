#ifndef SERIALIZE_H
#define SERIALIZE_H

#include "settings.h"

#define SAVED_PARAM(s, x)                                                      \
    { C_##s##_##x##_TAG, (long)(&(((s *)NULL)->x)), (sizeof(((s *)NULL)->x)) }

int write_settings_file(int file, settings_t *px_settings);
int read_settings_file(int file, settings_t *px_settings);

#endif // SERIALIZE_H

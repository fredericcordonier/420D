
#ifndef SERIALIZE_H
#define SERIALIZE_H

#include "settings.h"
#include "fcache.h"

int write_settings_file(fcache *cache);
int read_settings_file(fcache *cache);

#endif // SERIALIZE_H

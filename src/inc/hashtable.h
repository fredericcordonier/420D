/* hashtable.h */
#ifndef HASHTABLE_H
#define HASHTABLE_H

// This is a very basic implementation of read-only (no insertion, no dynamic allocation) hashtables with unsigned int keys and int values

#include <stdint.h>
#include "param_def.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Look up value by key. Returns 1 if found and writes *out_value, else 0. */
int hashtable_get(uint32_t key, int *out_value, const field_def_t *k_table, size_t k_count);

#ifdef __cplusplus
}
#endif

#endif /* HASH_TABLE_H */


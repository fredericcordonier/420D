// Implement serialization


#include "vxworks.h" /* size_t */
#include "hashtable.h"
#include "param_def.h"

// 1) Sorted array + binary search (tiny & robust)

// Minimal code & memory, excellent for embedded.
// Deterministic and easy to unit test.
// Updates are a simple regenerate-and-sort step.
// O(log N), for N≤400 it’s very fast in practice.

typedef struct {
    uint32_t key;
    int value;
} kv_pair_t;

/* Ensure the array k_table is sorted by key in ascending order. */
int hashtable_get(uint32_t key, int *out_value, const field_def_t *k_table, size_t k_count)
{
    size_t lo = 0, hi = (k_count == 0) ? 0 : (k_count - 1);
    while (lo <= hi && k_count > 0) {
        size_t mid = lo + ((hi - lo) >> 1);
        uint32_t mid_key = k_table[mid].i_field_name_hash;
        if (key == mid_key) {
            if (out_value) *out_value = &(k_table[mid]) - k_table;
            return 1;
        } else if (key < mid_key) {
            if (mid == 0) break; /* prevent size_t underflow */
            hi = mid - 1;
        } else { /* key > mid_key */
            lo = mid + 1;
        }
    }
    return 0;
}

// =============================================================================================================

// 2) Read‑only open addressing (linear probing, const tables)

// How to generate the const table (one-time at build time)
// You can write a tiny generator (C, Python, or even a pre-build step in your build system) that:


// Picks TABLE_SIZE = next_power_of_two(ceil(N / LOAD_FACTOR)), with LOAD_FACTOR ≈ 0.7–0.8.
// For N=400, a good choice is TABLE_SIZE=512 (~78% full).


// Fills arrays with EMPTY_KEY.


// For each (key, value) in your dataset (no duplicates), computes idx = hash(key) & (TABLE_SIZE-1) and linearly probes to the next free slot, placing key and value. If you ever loop too much:

// try a different hash seed or
// increase TABLE_SIZE.



// Emits C files (ro_ht.c fragment or separate ro_ht_data.c) with const arrays.


// This is reliable and fast, and you don’t need any third-party tools.
// Pros

// O(1) average lookup, very low constant time.
// Deterministic, no malloc, code is tiny.

// Cons

// Requires a small codegen step to bake the tables (but that step is simple).



// ======================================================================================================
// ======================================================================================================
// ======================================================================================================
// ======================================================================================================

#if 0
#define MULTIPLIER      31  // or 37 (empiric values)

// A basic hash function for strings
unsigned int hash(char *str)
{
   unsigned int h;
   unsigned char *p;

   h = 0;
   for (p = (unsigned char*)str; *p != '\0'; p++)
      h = MULTIPLIER * h + *p;
   return h; // or, h % ARRAY_SIZE;
}
#endif // Not needed in code (yet?) --> just for information

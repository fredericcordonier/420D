#ifndef PERSIST_H_
#define PERSIST_H_

#define PERSIST_FILENAME "PERSIST"
#define PERSIST_VERSION 0x01

#include "exposure.h"
#include "scripts.h"

#ifdef __cplusplus
extern "C" {
#endif


#define C_NB_AF_PATTERNS 3


typedef struct {
    ec_t ev_comp;                        // Exposure compensation for AutoISO + M
    script_t last_script;                // Last executed script
    int last_page;                       // Last page visited in main menu
    int aeb;                             // AEB being used
    int last_aeb;                        // Last AEB used
    int named_temps_top_of_page_line;    // Named temps top of page line
    int named_temps_cur_line;            // Named temps current line
    int af_patterns[C_NB_AF_PATTERNS];   // AF patterns being used
    int current_af_pattern;              // Current AF pattern
} persist_t;

extern persist_t persist;

extern int persist_read(void);
extern void persist_write(void);

#ifdef __cplusplus
}
#endif

#endif

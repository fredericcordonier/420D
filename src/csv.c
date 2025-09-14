/**
 * \file csv.c Simple .CSV file parser for named temperatures.
 *
 */

#include "ioLib.h"
#include "vxworks.h"
#include <ctype.h>
#include <string.h>

#include "firmware/fio.h"

#include "csv.h"
#include "debug.h"
#include "utils.h"
// #include "languages.h"

#define RET_OK 0
#define RET_FAIL 1

/* Strip whitespace chars off end of given string, in place. Return s. */
static char *rstrip(char *s) {
    char *p = s + strlen(s);
    while (p > s && isspace(*--p))
        *p = '\0';
    return s;
}

/* Return pointer to first non-whitespace char in given string. */
static char *lskip(const char *s) {
    while (*s && isspace(*s))
        s++;
    return (char *)s;
}

static int loadValues(char *line, char *pc_x_fields[]) {
    if (line == NULL)
        return RET_FAIL;

    char *cptr = line;
    int fld = 0;
    int inquote = FALSE;
    char ch;

    pc_x_fields[fld] = cptr;
    while (((ch = *cptr) != '\0') && (fld < NUM_FIELDS)) {
        if (ch == '"') {
            if (!inquote)
                pc_x_fields[fld] = cptr + 1;
            else {
                *cptr = '\0'; // zero out " and jump over it
            }
            inquote = !inquote;
        } else if (ch == CSV_DELIM && !inquote) {
            *cptr = '\0'; // end of field, null terminate it
            pc_x_fields[++fld] = cptr + 1;
        }
        cptr++;
    }
    if (fld < NUM_FIELDS - 1) {
        return RET_FAIL;
    }
    return RET_OK;
}

int csv_parse_file(int file, int start_at, csv_line_handler handler,
                   void *user) {
    char line[MAX_CSV_LINE];
    // char* array will point to fields
    char *pFields[NUM_FIELDS];

    char *start;
    int lineno = 0;

    if (file == -1)
        return RET_FAIL;

    hack_fgets_init();

    /* Scan through file line by line */
    while (hack_fgets(line, sizeof(line), file) != NULL) {

        start = lskip(rstrip(line));
        if (*start == '#') {
            // skip comment
            continue;
        }
        // go to starting line
        if (++lineno <= start_at)
            continue;

        if (loadValues(start, pFields) == RET_FAIL) {
            break;
        } else {
            // On return pFields array pointers point to loaded fields ready for
            // load into DB or whatever Fields can be accessed via pFields, e.g.
            if (handler) {
                if (!handler(user, lineno, pFields[NAME], pFields[VALUE],
                             pFields[MIN_VAL], pFields[MAX_VAL])) {
                    // handler returns 0, means stop here
                    break;
                }
            }
        }
    }
    return lineno;
}

/* See documentation in header file. */
int csv_parse(const char *filename, int start_at, csv_line_handler handler,
              void *user) {
    int error;

    int file = -1;

    if ((file = FIO_OpenFile(filename, O_RDONLY)) == -1)
        return -1;

    error = csv_parse_file(file, start_at, handler, user);

    FIO_CloseFile(file);

    return error;
}

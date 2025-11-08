/**
 * file csv.h
 * csv.h: Simple .CSV file parser
 *
 */

#ifndef __CSV_H__
#define __CSV_H__

#include "firmware.h"
#include "languages.h"

#define MAX_CSV_LINE 100
#define MAX_NAME LP_MAX_WORD
#define CSV_DELIM ';'
enum {
    NAME,
    VALUE,
    MIN_VAL,
    MAX_VAL,
    NUM_FIELDS

};

typedef int (*csv_line_handler)(void *user, int lineno, const char *name,
                                const char *value, char *min_value,
                                char *max_value);

/* Parse given CSV-style file. Shall have no header, but lines with four values,
   separater with ';' (semicolon).

   1. name of color temp
   2. int value of the color temp
   3. int min value of the color temp for adjustment
   4. int max value of the color temp for adjustment

   For each line parsed, call handler function with given user pointer as well
   as name, and values. Handler should return nonzero on success, zero on error.

   Returns 0 on success, line number of first error on parse error (doesn't
   stop on first error), or -1 on file open error.
*/

int csv_parse(const char *filename, int start_line, csv_line_handler handler,
              void *user);

/* Same as ini_parse(), but takes a FD instead of filename. This doesn't
   close the file when it's finished -- the caller must do that. */
int csv_parse_file(int fd, int start_at, csv_line_handler handler, void *user);

#endif /* __CSV_H__ */

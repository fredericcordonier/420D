#include "serialize.h"
#include "firmware/fio.h"
#include "ini.h"
#include "macros.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Prototypes for static functions
static int handle_line(void *user, int lineno, const char *section, char *name,
                       char *value);
static int handle_section(void *user, int lineno, const char *section);

// Struct used to define a saved parameter
typedef struct {
    char *param_name;
    int param_addr_offset;
    int nb_values;
} param_def;

// Get the list of parameters in settings structure
#define PARAM_INT_DEF(s, f) {#f, (long)(&(((s *)NULL)->f)), 1},
#define PARAM_INT_ARRAY_DEF(s, f, i) {#f, (long)(&(((s *)NULL)->f)), i},

const param_def my_parameters[] = {
#include "settings.def"
    {NULL, 0}};

#undef PARAM_INT_DEF
#undef PARAM_INT_ARRAY_DEF

// Write settings into ini file
int write_settings_file(int file, settings_t *px_settings) {
#define MAX_BUF 100
    const param_def *param_pt = my_parameters;
    int param_value;
    int ret_value;
    char buf[MAX_BUF];

    strcpy(buf, "[settings]\n");
    ret_value = FIO_WriteFile(file, buf, strlen(buf));
    if (ret_value != -1) {
        while ((param_pt->param_name != NULL) && (ret_value != -1)) {
            sprintf(buf, "%-30s", param_pt->param_name);
            int offset = (param_pt->param_addr_offset / sizeof(int));
            if (param_pt->nb_values > 1) {
                sprintf(buf + strlen(buf), "[%i]: ", param_pt->nb_values);
                for (int i = 0; i < param_pt->nb_values - 1; i++) {
                    param_value = *((int *)(px_settings) + offset);
                    offset += 1;
                    sprintf(buf + strlen(buf), "%i,", param_value);
                }
            } else {
                sprintf(buf + strlen(buf), "   : ");
            }
            param_value = *((int *)(px_settings) + offset);
            sprintf(buf + strlen(buf), "%i\n", param_value);
            ret_value = FIO_WriteFile(file, buf, strlen(buf));
            param_pt++;
        }
    }
    return ret_value;
}

static int parse_param_name(char *name, param_def *result) {
    char *pt = name;
    char val_buf[10];
    char *val_buf_pt = val_buf;
    while ((*pt != '[') && (*pt != '\0')) {
        if (*pt == ' ') {
            *pt = '\0'; // After name, put terminator
        }
        pt++;
    }
    result->nb_values = 1;
    if (*pt == '[') {
        *pt++ = '\0'; // Skip [ char
        while ((*pt != ']') && (*pt != '\0')) {
            *val_buf_pt++ = *pt++;
        }
        result->nb_values = atoi(val_buf);
    }
    result->param_name = name;
    return 0;
}

// Read ini file: handle a line containing a parameter
static int handle_line(void *user, int lineno, const char *section, char *name,
                       char *value) {
    settings_t *px_settings = (settings_t *)user;
    const param_def *param_pt = my_parameters;
    param_def param_read;

    // Read param name, check if multi-value written in file
    parse_param_name(name, &param_read);
    while (param_pt->param_name != NULL) {
        if (strcmp(param_pt->param_name, param_read.param_name) == 0) {
            // Read what we can read, but not more than exptected by our
            // settings
            int nb_values = MIN(param_pt->nb_values, param_read.nb_values);
            int offset = (param_pt->param_addr_offset / sizeof(int));
            for (int val_cnt = 0; val_cnt < nb_values - 1; val_cnt++) {
                char *current_value = value;
                while ((*current_value != ',') && (*current_value != '\0')) {
                    current_value++;
                }
                *current_value = '\0';
                *((int *)(px_settings) + offset) = atoi(value);
                current_value += 1;
                value = current_value;
                offset += 1;
            }
            *((int *)(px_settings) + offset) = atoi(value);
            break;
        }
        param_pt++;
    }
    return 1;
}

// Read ini file: handle a section name
static int handle_section(void *user, int lineno, const char *section) {
    return 1;
}

// Read an ini file containing settings
int read_settings_file(int file, settings_t *px_settings) {
    int error;

    error = ini_parse_file(file, "settings", (ini_line_handler)handle_line,
                           handle_section, px_settings);

    return error;
}

#if 0
int main(int argc, char *argv[]) {
    FILE *ini_file;
    settings_t my_settings;
    settings_t rsett;
    memset(&my_settings, 0, sizeof(my_settings));
    my_settings.use_dpad = 1;
    my_settings.autoiso_enable = 2;
    my_settings.autoiso_miniso = 3;
    my_settings.autoiso_maxiso = 4;
    my_settings.autoiso_mintv = 20;
    my_settings.autoiso_maxav = 234;
    my_settings.autoiso_relaxed = 10649;
    ini_file = fopen("settings.ini", "w");
    write_settings_ini(ini_file, &my_settings);
    fclose(ini_file);

    ini_file = fopen("settings.ini", "r");
    read_settings_ini(ini_file, &rsett);
    fclose(ini_file);

    printf("%i\n", my_settings.use_dpad);
    printf("%i\n", my_settings.autoiso_enable);
    printf("%i\n", my_settings.autoiso_miniso);
    printf("%i\n", my_settings.autoiso_maxiso);
    printf("%i\n", my_settings.autoiso_mintv);
    printf("%i\n", my_settings.autoiso_maxav);
    printf("%i\n", my_settings.autoiso_relaxed);

    return 0;
}
#endif // SIMUL

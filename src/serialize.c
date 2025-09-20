#include "serialize.h"
#include "firmware/fio.h"
#include "ini.h"
#include "macros.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Struct used to define a structure field
typedef struct {
    char *param_name;
    int param_addr_offset;
    int nb_values;
} field_def;

// Prototypes for static functions
static int serialize_structure(int file, int *px_data,
                               const field_def *fields_structure,
                               const char *struct_name);
static int handle_settings_line(void *user, int lineno, const char *section,
                                char *name, char *value);
static int handle_menu_order_line(void *user, int lineno, const char *section,
                                  char *name, char *value);
static int handle_section(void *user, int lineno, const char *section);

// Get the list of parameters in settings structure
#define PARAM_INT_DEF(s, f) {#f, (long)(&(((s *)NULL)->f)), 1},
#define PARAM_INT_ARRAY_DEF(s, f, i) {#f, (long)(&(((s *)NULL)->f)), i},

const field_def settings_structure[] = {
#include "settings.def"
    {NULL, 0}};

const field_def menu_order_structure[] = {
#include "menu_order.def"
    {NULL, 0}};
#undef PARAM_INT_DEF
#undef PARAM_INT_ARRAY_DEF

static int serialize_structure(int file, int *px_data,
                               const field_def *fields_structure,
                               const char *struct_name) {
#define MAX_BUF 100
    const field_def *p_field = fields_structure;
    int param_value;
    int ret_value;
    char buf[MAX_BUF];

    sprintf(buf, "[%s]\n", struct_name);
    ret_value = FIO_WriteFile(file, buf, strlen(buf));
    if (ret_value != -1) {
        while ((p_field->param_name != NULL) && (ret_value != -1)) {
            sprintf(buf, "%-30s", p_field->param_name);
            int offset = (p_field->param_addr_offset / sizeof(int));
            if (p_field->nb_values > 1) {
                sprintf(buf + strlen(buf), "[%2i]: ", p_field->nb_values);
                for (int i = 0; i < p_field->nb_values - 1; i++) {
                    param_value = *(px_data + offset);
                    offset += 1;
                    sprintf(buf + strlen(buf), "%i,", param_value);
                }
            } else {
                sprintf(buf + strlen(buf), "   : ");
            }
            param_value = *(px_data + offset);
            sprintf(buf + strlen(buf), "%i\n", param_value);
            ret_value = FIO_WriteFile(file, buf, strlen(buf));
            p_field++;
        }
    }
    return ret_value;
}

// Write settings into ini file
int write_settings_file(int file, settings_t *px_settings) {
    return serialize_structure(file, (int *)px_settings, settings_structure,
                               "settings");
}

// Write menu_order into ini file
int write_menu_order_file(int file, menu_order_t *px_menu_order) {
    return serialize_structure(file, (int *)px_menu_order, menu_order_structure,
                               "menu_order");
}

// Parse the first part of the ini file line (before the '=' sign).
//  It contains the name of the structure field, plus potentially the
//  size in int if higher than 1 (in the form [nb])
static int parse_field_name(char *name, field_def *result) {
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
            if (*pt != ' ') {
                *val_buf_pt++ = *pt;
            }
            pt++;
        }
        result->nb_values = atoi(val_buf);
    }
    result->param_name = name;
    return 0;
}

// Read ini file: handle a line containing a parameter
static int handle_data_line(void *px_data_struct, int lineno,
                            const char *section, char *name, char *value,
                            const field_def *struct_definition) {
    const field_def *p_field = struct_definition;
    field_def param_read;

    // Read param name, check if multi-value written in file
    parse_field_name(name, &param_read);
    while (p_field->param_name != NULL) {
        if (strcmp(p_field->param_name, param_read.param_name) == 0) {
            // Read what we can read, but not more than exptected by our
            // settings
            int nb_values = MIN(p_field->nb_values, param_read.nb_values);
            int offset = (p_field->param_addr_offset / sizeof(int));
            for (int val_cnt = 0; val_cnt < nb_values - 1; val_cnt++) {
                char *current_value = value;
                while ((*current_value != ',') && (*current_value != '\0')) {
                    current_value++;
                }
                *current_value = '\0';
                *((int *)(px_data_struct) + offset) = atoi(value);
                current_value += 1;
                value = current_value;
                offset += 1;
            }
            *((int *)(px_data_struct) + offset) = atoi(value);
            break;
        }
        p_field++;
    }
    return 1;
}

// Read one line of settings structure from ini file
static int handle_settings_line(void *user, int lineno, const char *section,
                                char *name, char *value) {
    // Call generic function with settings structure
    return handle_data_line(user, lineno, section, name, value,
                            settings_structure);
}

// Read one line of menu_order structure from ini
static int handle_menu_order_line(void *user, int lineno, const char *section,
                                  char *name, char *value) {

    return handle_data_line(user, lineno, section, name, value,
                            menu_order_structure);
}

// Read ini file: handle a section name
static int handle_section(void *user, int lineno, const char *section) {
    return 1;
}

// Read an ini file containing settings
int read_settings_file(int file, settings_t *px_settings) {
    int error;

    error =
        ini_parse_file(file, "settings", (ini_line_handler)handle_settings_line,
                       handle_section, px_settings);

    return error;
}

// Read menu_orders in ini file
int read_menu_order_file(int file, menu_order_t *px_menu_order) {
    int error = -1;

    error = ini_parse_file(file, "menu_order",
                           (ini_line_handler)handle_menu_order_line,
                           handle_section, px_menu_order);
    return error;
}

#ifdef SIMUL
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

#include "serialize.h"
#include "firmware/fio.h"
#include "ini.h"
#include "macros.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"
#include "param_def.h"
#include "settings_t.h"
#include "menu_order_t.h"

typedef struct {
    int tag;        // Tag to identify the structure (e.g. settings_t, menu_order_t)
    int len;        // Size of data (number of bytes) stored in structure for this tag
} tag_len_t;

// Prototypes for static functions
static int serialize_structure(int file, unsigned char *pu_x_data, const list_field_def_t *ps_x_hash_table);
static void read_structure(int file, unsigned char *base_addr, const list_field_def_t *fields, int data_size);

static int serialize_structure(int file, unsigned char *pu_x_data,
                               const list_field_def_t *ps_x_hash_table) {
#define MAX_BUF 100
    const field_def_t *ps_l_field = ps_x_hash_table->data;
    int i_l_write_result = 0;
    int i_l_nb_int_written = 0;
    int i_l_field_cnt;
    tag_len_t s_l_tag_len;

    for (i_l_field_cnt = 0; (i_l_field_cnt < ps_x_hash_table->size) && (i_l_write_result != -1); i_l_field_cnt++) {
        s_l_tag_len.tag = ps_l_field->i_field_name_hash;
        s_l_tag_len.len = ps_l_field->i_field_size;
        int i_l_offset = (ps_l_field->i_field_offset_in_struct);
        // Write metadata for field
        i_l_write_result = FIO_WriteFile(file, &s_l_tag_len, sizeof(tag_len_t));
        // Write field data
        i_l_write_result = FIO_WriteFile(file, (pu_x_data + i_l_offset), (size_t)(s_l_tag_len.len));
        i_l_nb_int_written += (size_t)sizeof(tag_len_t) + (size_t)(s_l_tag_len.len);
        ps_l_field++;
    }
    return i_l_nb_int_written;
}

// Write settings into ini file
int write_settings_file(int file) {
    int val = 0;
    tag_len_t s_l_tag_len;
    s_l_tag_len.tag = C_SETTINGS_T_TAG;
    s_l_tag_len.len = sizeof(settings) + (s_g_settings_t_hashtable.size) * (sizeof(field_def_t) - sizeof(int));                // Size of structure data + size of metadata (tag & len)
    FIO_WriteFile(file, &s_l_tag_len, sizeof(tag_len_t));
    int result = serialize_structure(file, (unsigned char *)(&settings), &s_g_settings_t_hashtable);
    if (result == val) {
        // Success writing
        s_l_tag_len.tag = C_MENU_ORDER_T_TAG;
        s_l_tag_len.len = sizeof(menu_order) + (s_g_menu_order_t_hashtable.size) * (sizeof(field_def_t) - sizeof(int));        // Size of structure data + size of metadata (tag & len)
        FIO_WriteFile(file, &s_l_tag_len, sizeof(tag_len_t));
        result += serialize_structure(file, (unsigned char *)(&menu_order), &s_g_menu_order_t_hashtable);
    }
    return result;
}


static void read_structure(int i_x_file, unsigned char *pu_x_base_addr, const list_field_def_t *ps_x_fields, int i_l_data_size) {
    int i_l_success = 0;
    int i_l_total_read = 0;
    int i_l_nb_read = 0;
    int i_l_expected = 0;               // Size of data stored in structure (expected number of values)
    int i_l_def_offset = 0;             // Offset of field in the field_def_t structure (hash table)
    int i_l_struct_offset = 0;          // Offset of data in the structure to be filled
    tag_len_t s_l_tag_len;

    i_l_total_read = FIO_ReadFile(i_x_file, &s_l_tag_len, sizeof(tag_len_t));
    while (i_l_total_read < i_l_data_size)
    {
        i_l_success = hashtable_get(s_l_tag_len.tag, &i_l_def_offset, ps_x_fields->data, ps_x_fields->size);
        if (i_l_success) {
            i_l_expected = ps_x_fields->data[i_l_def_offset].i_field_size;
            i_l_struct_offset = ps_x_fields->data[i_l_def_offset].i_field_offset_in_struct;
            // Ensure we don't read too many values, neither from file, not to store in struct
            i_l_expected = MIN(i_l_expected, s_l_tag_len.len);
            i_l_total_read += FIO_ReadFile(i_x_file, (pu_x_base_addr + i_l_struct_offset), i_l_expected);
            if (s_l_tag_len.len > i_l_expected) {
                i_l_nb_read = (s_l_tag_len.len - i_l_expected);
                FIO_SeekFile(i_x_file, i_l_nb_read, SEEK_CUR);
                i_l_total_read += i_l_nb_read;
            }
        }
        if (i_l_total_read < i_l_data_size) {
            i_l_nb_read = FIO_ReadFile(i_x_file, &s_l_tag_len, sizeof(tag_len_t));
            i_l_total_read += i_l_nb_read;
        }
    }
}

// Read an ini file containing settings
int read_settings_file(int file) {
    tag_len_t read_tag_len;
    int read_len;

    read_len = FIO_ReadFile(file, &read_tag_len, sizeof(tag_len_t));
    while (read_len ==  sizeof(tag_len_t)) {
        switch (read_tag_len.tag)
        {
            case C_SETTINGS_T_TAG:
            read_structure(file, (unsigned char *)(&settings), &s_g_settings_t_hashtable, read_tag_len.len);
            break;
            case C_MENU_ORDER_T_TAG:
            read_structure(file, (unsigned char *)(&menu_order), &s_g_menu_order_t_hashtable, read_tag_len.len);
            break;
            default:
            FIO_SeekFile(file, read_tag_len.len * sizeof(int), SEEK_CUR);
            break;
        }
        read_len = FIO_ReadFile(file, &read_tag_len, sizeof(tag_len_t));
    }

    return 1;
}


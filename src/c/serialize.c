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
    int tag;
    int len;
} tag_len_t;

// Prototypes for static functions
static int serialize_structure(int file, unsigned char *pu_x_data, const list_field_def_t *ps_x_hash_table);
static void read_structure(int file, unsigned char *base_addr, const list_field_def_t *fields, int data_size);

static int serialize_structure(int file, unsigned char *pu_x_data,
                               const list_field_def_t *ps_x_hash_table) {
#define MAX_BUF 20
    const field_def_t *ps_l_field = ps_x_hash_table->data;
    int i_l_param_value;
    int i_l_write_result = 0;
    int ai_l_buf[MAX_BUF];
    int *pi_l_buf = ai_l_buf;
    int i_l_nb_int_written = 0;
    int i_l_field_cnt;

    for (i_l_field_cnt = 0; (i_l_field_cnt < ps_x_hash_table->size) && (i_l_write_result != -1); i_l_field_cnt++) {
        *pi_l_buf++ = ps_l_field->i_field_name_hash;
        *pi_l_buf++ = (ps_l_field->i_field_size);
        int i_l_offset = (ps_l_field->i_field_offset_in_struct);
        for (int i_l = 0; i_l < ps_l_field->i_field_size; i_l++) {
            i_l_param_value = (int)(*(pu_x_data + i_l_offset));
            i_l_offset += sizeof(int);
            *pi_l_buf++ = i_l_param_value;
        }
        i_l_write_result = FIO_WriteFile(file, ai_l_buf, (size_t)((pi_l_buf - ai_l_buf) * sizeof(int)));
        i_l_nb_int_written += (size_t)(ps_l_field->i_field_size + 2);
        ps_l_field++;
        pi_l_buf = ai_l_buf;
    }
    return i_l_nb_int_written;
}

// Write settings into ini file
int write_settings_file(int file) {
    int val = 0;
    val = C_SETTINGS_T_TAG;
    FIO_WriteFile(file, &val, sizeof(int));
    val = sizeof(settings) / sizeof(int) + (s_g_settings_t_hashtable.size) * 2;         // Size of structure data + size of metadata (tag & len)
    FIO_WriteFile(file, &val, sizeof(int));
    int result = serialize_structure(file, (unsigned char *)(&settings), &s_g_settings_t_hashtable);
    if (result == val) {
        // Success writing
        val = C_MENU_ORDER_T_TAG;
        FIO_WriteFile(file, &val, sizeof(int));
        val = sizeof(menu_order_t) / sizeof(int) + (s_g_menu_order_t_hashtable.size) * 2;
        FIO_WriteFile(file, &val, sizeof(int));
        result += serialize_structure(file, (unsigned char *)(&menu_order), &s_g_menu_order_t_hashtable);
    }
    return result;
}


static void read_structure(int i_x_file, unsigned char *pu_x_base_addr, const list_field_def_t *ps_x_fields, int i_l_data_size) {
    int i_l_success = 0;
    int i_l_total_read = 0;
    int i_l_nb_read = 0;
    int i_l_expected = 0;          // Nb of int to be stored in structure (expected number of values)
    int i_l_def_offset = 0;             // Offset of field in the field_def_t structure (hash table)
    int i_l_struct_offset = 0;          // Offset of data in the structure to be filled
    tag_len_t s_l_tl;
    int i_l_value;

    i_l_total_read = FIO_ReadFile(i_x_file, &s_l_tl, sizeof(tag_len_t));
    while (i_l_total_read < i_l_data_size)
    {
        i_l_success = hashtable_get(s_l_tl.tag, &i_l_def_offset, ps_x_fields->data, ps_x_fields->size);
        if (i_l_success) {
            i_l_expected = ps_x_fields->data[i_l_def_offset].i_field_size;
            i_l_struct_offset = ps_x_fields->data[i_l_def_offset].i_field_offset_in_struct;
            // Ensure we don't read too many values, neither from file, not to store in struct
            i_l_expected = MIN(i_l_expected, s_l_tl.len);
            for (int i_l = 0; i_l < i_l_expected; i_l++) {
                i_l_total_read += FIO_ReadFile(i_x_file, &i_l_value, sizeof(int));
                *((int *)(pu_x_base_addr + i_l_struct_offset)) = i_l_value;
                i_l_struct_offset += sizeof(int);
            }
            if (s_l_tl.len > i_l_expected) {
                i_l_nb_read = (s_l_tl.len - i_l_expected) * sizeof(int);
                FIO_SeekFile(i_x_file, i_l_nb_read, SEEK_CUR);
                i_l_total_read += i_l_nb_read;
            }
        }
        if (i_l_total_read < i_l_data_size) {
            i_l_nb_read = FIO_ReadFile(i_x_file, &s_l_tl, sizeof(tag_len_t));
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
            read_structure(file, (unsigned char *)(&settings), &s_g_settings_t_hashtable, read_tag_len.len * sizeof(int));
            break;
            case C_MENU_ORDER_T_TAG:
            read_structure(file, (unsigned char *)(&menu_order), &s_g_menu_order_t_hashtable, read_tag_len.len * sizeof(int));
            default:
            FIO_SeekFile(file, read_tag_len.len * sizeof(int), SEEK_CUR);
            break;
        }
        read_len = FIO_ReadFile(file, &read_tag_len, sizeof(tag_len_t));
    }

    return 1;
}


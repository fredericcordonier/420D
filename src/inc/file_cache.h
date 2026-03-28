/*
 * fcache.h
 *
 *  Created on: 16 Apr 2024
 *      Author: cordonie
 */

#pragma once


typedef struct {
    int    file;
    char  *buf;
    size_t cur_pos;
    size_t size;
    int    mode;
}file_cache_t;

int  i_g_file_cache_open(file_cache_t *ps_x_cache, int i_x_fd, char *pc_x_buf, size_t i_x_size);
int  i_g_file_cache_read(file_cache_t *ps_x_cache, void *p_x_buffer, size_t i_x_count);
int  i_g_fcache_write(file_cache_t *ps_x_cache, void *p_x_buffer, size_t i_x_count);
void g_file_cache_close_file(file_cache_t *ps_x_cache);

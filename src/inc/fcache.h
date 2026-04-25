/*
 * fcache.h
 *
 *  Created on: 16 Apr 2024
 *      Author: cordonie
 */

#pragma once

#include <ioLib.h>
#include <string.h>
#include <vxworks.h>


typedef struct {
    int    i_file;
    char  *pc_buf;
    size_t i_cur_pos;
    size_t i_size;
    int    i_mode;
}fcache;

int  fcache_open(fcache *self, const char *pc_x_filename, void *p_x_buffer, size_t i_x_size, int i_x_mode);
int  fcache_read(fcache *cache, void *buffer, size_t count);
int  fcache_write(fcache *self, void *buffer, size_t count);
void fcache_close_file(fcache *cache);
void fcache_seek(fcache *cache, size_t offset, int whence);

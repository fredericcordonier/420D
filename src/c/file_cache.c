/*
 * fcache.c
 *
 *  Created on: 16 Apr 2024
 *      Author: cordonie
 */

#include <string.h>
#include "firmware/fio.h"
#include "ioLib.h"
#include "macros.h"
#include "file_cache.h"

/**
 * @brief Open a file and initialize the read/write buffer.
 * @param ps_x_cache Pointer to a fcache structure (owned by the caller, to avoid dynamic memory allocation).
 * @param file Id of file to open.
 * @param buffer   Buffer (owned by called to avoid dynamic allocation)
 * @param size     Size of provided buffer
 * @return
 */
int  i_g_file_cache_open(file_cache_t *ps_x_cache, int i_x_fd, char *pc_x_buf, size_t i_x_size) {
    ps_x_cache->file = i_x_fd;
    ps_x_cache->buf = pc_x_buf;
    ps_x_cache->size = i_x_size;
    ps_x_cache->cur_pos = i_x_size;                 // At init, ready to fill the buffer, as if it was entirely read
    return ps_x_cache->file;
}

/**
 * @brief Read from file with buffer
 * @param ps_x_cache   Pointer to fcache structure, owned by caller
 * @param buffer Pointer to data to read, memory owned by caller
 * @param count  Number of bytes to read
 * @return       Number of bytes read
 */
int  i_g_file_cache_read(file_cache_t *ps_x_cache, void *p_x_buffer, size_t i_x_count) {
    int i_l_size;
    int i_l_read_len;
    int i_l_remaining = i_x_count;
    do {
        i_l_read_len = MIN(i_l_remaining, ps_x_cache->size - ps_x_cache->cur_pos);
        memcpy(p_x_buffer, (void *)(ps_x_cache->buf + ps_x_cache->cur_pos), i_l_read_len);
        p_x_buffer = (void *)((char *)p_x_buffer + i_l_read_len);
        i_l_remaining -= i_l_read_len;
        if (i_l_remaining > 0) {
            // The entire buffer was read, refill
            if ((i_l_size = FIO_ReadFile(ps_x_cache->file, (void *)ps_x_cache->buf, ps_x_cache->size)) != ps_x_cache->size) {
                // Could not read the entire buffer, adjust the size of available data
                if (i_l_size < (i_x_count - i_l_read_len))
                    // File did not feed enough data, adjust the number of bytes read
                    i_x_count = i_l_size+i_l_read_len;
            }
            ps_x_cache->size = i_l_size;
            ps_x_cache->cur_pos = 0;
        }
        else
            ps_x_cache->cur_pos += i_l_read_len;
    } while (i_l_remaining > 0);

    return i_x_count;
}

int  i_g_fcache_write(file_cache_t *ps_x_cache, void *p_x_buffer, size_t i_x_count) {
    int i_l_size;
    int i_l_remaining = i_x_count;
    int i_l_write_len;
    if ((ps_x_cache == NULL) || (p_x_buffer == NULL) || (ps_x_cache->mode != O_WRONLY) || (ps_x_cache->file == -1)) {
        return -1;
    }
    do {
        i_l_write_len = MIN(i_l_remaining, ps_x_cache->size - ps_x_cache->cur_pos);
        memcpy((void *)(ps_x_cache->buf + ps_x_cache->cur_pos), p_x_buffer, i_l_write_len);
        p_x_buffer = (void *)((char *)p_x_buffer + i_l_write_len);
        i_l_remaining -= i_l_write_len;
        if (i_l_remaining > 0) {
            // The entire buffer was read, refill
            if ((i_l_size = FIO_WriteFile(ps_x_cache->file, ps_x_cache->buf, ps_x_cache->size)) != ps_x_cache->size) {
                // Could not write the entire buffer, return the number of bytes written
                return i_x_count-i_l_remaining;
            }
            ps_x_cache->cur_pos = 0;
        }
        else
            ps_x_cache->cur_pos += i_l_write_len;
    } while (i_l_remaining > 0);

    return i_x_count;
}

void g_file_cache_close_file(file_cache_t *ps_x_cache) {
    if ((ps_x_cache->mode == O_WRONLY) && ps_x_cache->cur_pos != 0) {
        FIO_WriteFile(ps_x_cache->file, ps_x_cache->buf, ps_x_cache->cur_pos);
    }
    FIO_CloseFile(ps_x_cache->file);
}

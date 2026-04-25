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
#include "fcache.h"

/**
 * @brief Open a file and initialize the read/write buffer.
 * @param self Pointer to a fcache structure (owned by the caller, to avoid dynamic memory allocation).
 * @param filename Name of file to open.
 * @param buffer   Buffer (owned by called to avoid dynamic allocation)
 * @param size     Size of provided buffer
 * @param mode     File open mode (O_RDONLY or O_WRONLY are supported)
 * @return
 */
int fcache_open(fcache *self, const char *filename, void *buffer, size_t size, int mode)
{
    if ((self->i_file = FIO_OpenFile(filename, mode)) == -1)
    {
        return -1;
    }
    self->pc_buf = buffer;
    self->i_mode = mode;
    self->i_size = size;
    if (mode == O_RDONLY) {
        self->i_cur_pos = size; // No data read, as if the buffer was entirely read
    }
    else
        self->i_cur_pos = 0;
    return self->i_file;
}

/**
 * @brief Read from file with buffer
 * @param self   Pointer to fcache structure, owned by caller
 * @param buffer Pointer to data to read, memory owned by caller
 * @param count  Number of bytes to read
 * @return       Number of bytes read
 */
int fcache_read(fcache *self, void *buffer, size_t count)
{
    int size;
    int read_len;
    int remaining = count;
    if ((self == NULL) || (buffer == NULL) || (self->i_mode != O_RDONLY) || (self->i_file == -1))
    {
        return -1;
    }
    do
    {
        read_len = MIN(remaining, self->i_size - self->i_cur_pos);
        memcpy(buffer, (void *)(self->pc_buf + self->i_cur_pos), read_len);
        buffer = (void *)((char *)buffer + read_len);
        remaining -= read_len;
        if (remaining > 0)
        {
            // The entire buffer was read, refill
            if ((size = FIO_ReadFile(self->i_file, (void *)self->pc_buf, self->i_size)) != self->i_size)
            {
                // Could not read the entire buffer, adjust the size of available data
                if (size == 0)  {
                  remaining = 0;
                  count = read_len;
                }
            }
            self->i_size = size;
            self->i_cur_pos = 0;
        }
        else
            self->i_cur_pos += read_len;
    } while (remaining > 0);

    return count;
}

int fcache_write(fcache *self, void *buffer, size_t count)
{
    int size;
    int remaining = count;
    int write_len;
    if ((self == NULL) || (buffer == NULL) || (!(self->i_mode & O_WRONLY)) || (self->i_file == -1))
    {
        return -1;
    }
    do
    {
        write_len = MIN(remaining, self->i_size - self->i_cur_pos);
        memcpy((void *)(self->pc_buf + self->i_cur_pos), buffer, write_len);
        buffer = (void *)((char *)buffer + write_len);
        remaining -= write_len;
        if (remaining > 0)
        {
            // The entire buffer was read, refill
            if ((size = FIO_WriteFile(self->i_file, self->pc_buf, self->i_size)) != self->i_size)
            {
                // Could not write the entire buffer, return the number of bytes written
                return count - remaining;
            }
            self->i_cur_pos = 0;
        }
        else
            self->i_cur_pos += write_len;
    } while (remaining > 0);

    return count;
}

void fcache_seek(fcache *cache, size_t offset, int whence) {
    if ((cache == NULL) || (cache->i_file == -1))
    {
        return;
    }
    if ((whence == SEEK_CUR) || (whence == SEEK_SET))
    {
        FIO_SeekFile(cache->i_file, offset, whence);
        if (cache->i_mode == O_RDONLY)
            cache->i_cur_pos = cache->i_size; // Ready to read data at that new position
        else
            cache->i_cur_pos = 0;
    }
}

void fcache_close_file(fcache *self)
{
    if ((self->i_mode & O_WRONLY) && self->i_cur_pos != 0)
    {
        FIO_WriteFile(self->i_file, self->pc_buf, self->i_cur_pos);
    }
    FIO_CloseFile(self->i_file);
}

#include <iostream>

#include <firmware/fio.h>
#include <vxworks/ioLib.h>
#include <algorithm>
#include <fstream>
#include <string>
#include "eos420d.h"
#include "utils.h"
#include "file_system.h"

FILE *FileSystem::get_fstream(int fd) {
    return files[fd];
}

void FileSystem::remove_file(int fd) {
    files.erase(fd);
}

FstreamTo420D::FstreamTo420D(FstreamTo420D&& other) noexcept {
    file_descriptor = other.file_descriptor;
    stream = other.stream;
    other.stream = nullptr;
}

FstreamTo420D& FstreamTo420D::operator=(FstreamTo420D&& other) noexcept {
    if (this != &other) {
        file_descriptor = other.file_descriptor;
        stream = other.stream;
        other.stream = nullptr;
    }
    return *this;
}

int  FIO_OpenFile(const char *filename, int mode) {
    FILE *f;
    std::string file_name_without_drive(filename);
    std::size_t pos = file_name_without_drive.find(':');
    if (pos != std::string::npos) {
        file_name_without_drive.erase(0, pos + 2);
    }
#ifdef _WIN32
    // On Windows, replace '/' with '\'
    std::replace(file_name_without_drive.begin(), file_name_without_drive.end(), '/', '\\');
#else
    // On Unix-like systems, replace '\' with '/'
    std::replace(file_name_without_drive.begin(), file_name_without_drive.end(), '\\', '/');
#endif
    std::cout << "Opening file " << file_name_without_drive << std::endl;
    if (mode == O_RDONLY) {
        f = fopen(file_name_without_drive.c_str(), "rb");
    }
    else {
        f = fopen(file_name_without_drive.c_str(), "wb");
    }
    if (f != NULL) {
        return my_camera.flash_420d.add_file(f);
    }
    else {
        return -1;
    }
}

int  FIO_ReadFile(int fd, void *buffer, size_t count) {
    FILE *stream = my_camera.flash_420d.get_fstream(fd);
    if (stream != nullptr) {
        return fread(buffer, 1, count, stream);
    }
    else {
        return 0;
    }
}

void FIO_SeekFile(int fd, long offset, int whence) {
    FILE *stream = my_camera.flash_420d.get_fstream(fd);
    if (stream != nullptr) {
        fseek(stream, offset, whence);
    }
}

int  FIO_WriteFile(int fd, void *buffer, size_t count) {
    FILE *stream = my_camera.flash_420d.get_fstream(fd);
    if (stream != nullptr) {
        return fwrite(buffer, count, 1, stream);
    }
    else {
        return 0;
    }
}

void FIO_CloseFile(int fd) {
    FILE *stream = my_camera.flash_420d.get_fstream(fd);
    if (stream != nullptr) {
        fclose(stream);
        my_camera.flash_420d.remove_file(fd);
    }
}

int FIO_RemoveFile(const char *filename) {
    return 0;
}

int FIO_CreateDirectory(const char *dirname)
{
    return 0;
}

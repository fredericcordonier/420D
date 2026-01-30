#ifndef FILE_SYSTEM_H
#define FILE_SYSTEM_H

#include <map>
#include <fstream>
#include <stdio.h>

class FstreamTo420D {
    public:
    FstreamTo420D(int file_desc, FILE *file): file_descriptor(file_desc), stream(file) {}
    // Prevent default copy constructors
    FstreamTo420D(const FstreamTo420D&) = delete;
    FstreamTo420D& operator=(const FstreamTo420D&) = delete;
    FstreamTo420D& operator=(FstreamTo420D&& other) noexcept;
    FstreamTo420D(FstreamTo420D&& other) noexcept;
    ~FstreamTo420D() {}
    int get_file_descriptor() {return file_descriptor;}
    FILE *get_stream_p() {return stream;}
    private:
    int file_descriptor;
    FILE *stream;
};

class FileSystem {
    public:
    FileSystem() {}
    ~FileSystem() {}
    int add_file(FILE *file) {
        int fd = files.size() + 1;
        files.emplace(std::make_pair(fd, file));
        return fd;
    }
    FILE *get_fstream(int fd);
    void remove_file(int fd);
    private:
    std::map<int , FILE *> files;
};

#endif

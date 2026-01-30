#ifndef INI_WRAP_H
#define INI_WRAP_H

#include <ini.h>

class IniFile {
    public:
    IniFile(const char *filename):filename(filename) {}
    ~IniFile() {}
    int parse(const char *wanted_section);
    static int section_found(void* user, int lineno, const char* section);
    static int handle_line(void* user, int lineno, const char* section, const char* name, const char* value);
    private:
    const char *filename;
};

#endif // INI_WRAP_H

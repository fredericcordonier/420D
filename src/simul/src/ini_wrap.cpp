#include <iostream>
#include <ini.h>
#include <ini_wrap.h>

int IniFile::parse(const char *wanted_section) {
    void *user = NULL;
    return ini_parse(filename, wanted_section, handle_line, section_found, user);
}

int IniFile::section_found(void* user, int lineno, const char* section) {
    std::cout << "Section found: [" << section << "] at line" << lineno << std::endl;
    return 1;
}

int IniFile::handle_line(void* user, int lineno, const char* section, const char* name, const char* value) {
    std::cout << "Handle line " << name << "=" << value << " in section [" << section << "] at line " << lineno << std::endl;
    return 1;
}

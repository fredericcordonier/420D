#include <iostream>
#include <fstream>
#include <string.h>
#include <firmware/fio.h>
#include <vxworks/ioLib.h>
#include "ini_wrap.h"
#include "eos420d.h"

#include "main.h"
#include "settings.h"

int main(int argc, char *argv[]) {
    char buf[10] = "test";
    std::cout << "Hello from UT main" << std::endl;
    // int fd = FIO_OpenFile("essai.ini", O_RDONLY);
    // std::cout << "File open: " << fd << std::endl;
    // FIO_ReadFile(fd, buf, 9);
    // buf[9] = '\0';
    // std::cout << "Data read: " << buf << std::endl;
    // int fd2 = FIO_OpenFile("test.txt", O_WRONLY);
    // std::cout << "File open: " << fd2 << std::endl;
    // char buf2[] = "Test data";
    // FIO_WriteFile(fd2, buf2, strlen(buf2));
    // FIO_CloseFile(fd);
    // FIO_CloseFile(fd2);
    IniFile inif = IniFile("essai.ini");
    inif.parse("hello");
    int result = settings_read();
    if (result == FALSE) {
        std::cout << "Could not open settings file" << std::endl;
        std::cout << "Result of settings_read(): " << result << std::endl;
    }
    settings_write();
}

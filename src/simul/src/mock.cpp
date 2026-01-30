#include <iostream>
#include <cstring>
#include <firmware/fio.h>
#include "eos420d.h"
#include "utils.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


char *hack_fgets_simple_but_slow(char *s, int n, int fd) {
    FILE *file = my_camera.flash_420d.get_fstream(fd);
    return fgets(s, n, file);
}


// char* strncpy0(char* dest, const char* src, size_t size) {
//     return strncpy(dest, src, size);
// }

// void stoupper(char *s) {
// 	while (*s) {
// 		if(('a' <= *s) && (*s <= 'z')) {
// 			*s = 'A' + (*s - 'a');
// 		}
// 		s++;
// 	}
// }

#ifdef __cplusplus
}
#endif // __cplusplus

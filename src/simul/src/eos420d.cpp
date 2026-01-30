#include <iostream>
#include <cstring>
#include "firmware/camera.h"
#include "eos420d.h"

EOS420D my_camera;

int RemReleaseInstMin;
int RemReleaseSelfMax;
dpr_data_t   DPData;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void GetLanguageStr(int lang_id, char * lang_str) {
    strcpy(lang_str, "ENGLISH");
}

#ifdef __cplusplus
}
#endif // __cplusplus

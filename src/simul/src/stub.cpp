#include <stdio.h>
#include <string.h>
#include "settings.h"
#include "languages.h"


char *strncpy0(char *dest, const char *src, size_t size) {
    return strncpy(dest, src, size);
}

void SleepTask(long msec)
{

}

long eventproc_EdLedOn(void)
{
    return 0;
}
long eventproc_EdLedOff(void)
{
    return 0;
}
long eventproc_EdLedBlink(void)
{
    return 0;
}

int eventproc_RemOn(void)
{
    return 0;
}
int eventproc_RemOff(void)
{
    return 0;
}

int send_to_intercom(int message, int parm) {
    return 0;
}
int InitIntercomData(void (*proxy)(const int, char *)) {
    return 0;
}
int IntercomHandler(const int handler, const char *message) {
    return 0;
}
int SendToIntercom(int message, int length, int parm) {
    return 0;
}
int IntercomHandlerButton(int button, int unknown) {
    return 0;
}
int SetTurnDisplayEvent_1_after_2(void) {
    return 0;
}
int SetTurnDisplayEvent_2_after_1(void) {
    return 0;
}

void EnterFactoryMode(void) {

}

void ExitFactoryMode(void) {

}

char lang_pack_current[L_COUNT][LP_MAX_WORD];

int RemPulseWidthMin;
int RemPulseWidthMax;
int RemPulseDiffMin;
int RemPulseDiffMax;
int RemReleaseInstMax;
int RemReleaseSelfMin;

/**
 * \file intercom.h
 * \brief Header for intercom.c
 */
#ifndef INTERCOM_H_
#define INTERCOM_H_

#ifdef __cplusplus
extern "C" {
#endif

#define INTERCOM_WAIT 1

void intercom_proxy   (const int handler, unsigned char *message);
int  send_to_intercom (int message, int parm);

#ifdef __cplusplus
}
#endif

#endif /* INTERCOM_H_ */

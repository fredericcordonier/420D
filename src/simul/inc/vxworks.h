#ifndef VXWORKS_VXWORKS_H
#define VXWORKS_VXWORKS_H
#include "firmware/fio.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int BOOL;
typedef int STATUS;

#define OK    (0)
#define ERROR (-1)

#define FALSE 0
#define TRUE  1

#define	STD_IN  0
#define	STD_OUT 1
#define	STD_ERR 2

typedef unsigned int  UINT;
typedef unsigned long ULONG;

//typedef long unsigned int size_t;
//typedef long unsigned int fpos_t;

typedef unsigned int  uint_t;
typedef unsigned long ulong_t;

typedef int  (*FUNCPTR)     (void);
typedef void (*VOIDFUNCPTR) (void);

#ifdef __cplusplus
}
#endif

#endif /* VXWORKS_VXWORKS_H */

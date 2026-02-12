#ifndef _ASM_H_
#define _ASM_H_

/*
 * Some ARM assembler macros for the cache hacks
 */

#define ASM_RET       0xE12FFF1E  // bx lr
#define ASM_FAR_CALL  0xE51FF004  // ldr pc, [pc,#-4]
#define ASM_LOOP      0xEAFFFFFE  // 1: b 1b
#define ASM_NOP       0xE1A00000  // mov r0, r0

#define ASM_MOV_R0_INT(value)  0
#define ASM_BL(pc,dest)        0
#define ASM_B(pc,dest)         0

#endif

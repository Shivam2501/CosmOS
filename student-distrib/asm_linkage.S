#define ASM     1

#include "x86_desc.h"

.globl asm_timer_chip
asm_timer_chip:
pushal
call TIMER_CHIP
popal
iret

.globl asm_keyboard
asm_keyboard:
pushal
call keyboard_handler
popal
iret


.globl asm_rtc
asm_rtc:
pushal
call rtc_handler
popal
iret

.globl asm_default
asm_default:
pushal
call INTERRUPT_DEFAULT
popal
iret

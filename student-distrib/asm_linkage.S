#define ASM     1

#include "x86_desc.h"

#calls handler for PIT
.globl asm_timer_chip
asm_timer_chip:
pushal
call pit_handler
popal
iret

#calls handler for keyboard
.globl asm_keyboard
asm_keyboard:
pushal
call keyboard_handler
popal
iret

#calls handler for mouse
.globl asm_mouse
asm_mouse:
pushal
call mouse_handler
popal
iret

#calls handler for RTC
.globl asm_rtc
asm_rtc:
pushal
call rtc_handler
popal
iret

#Calls handler for all other interrupts
.globl asm_default
asm_default:
pushal
call INTERRUPT_DEFAULT
popal
iret

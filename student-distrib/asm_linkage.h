#ifndef _ASM_LINKAGE_H
#define _ASM_LINKAGE_H

//calls handler for PIT
extern void asm_timer_chip(void);

//calls handler for keyboard
extern void asm_keyboard(void);

//calls handler for mouse
extern void asm_mouse(void);

//calls handler for RTC
extern void asm_rtc(void);

//Calls handler for all other interrupts
extern void asm_default(void);

#endif

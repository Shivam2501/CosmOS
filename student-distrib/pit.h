#ifndef _PIT_H
#define _PIT_H

#include "types.h"
#include "i8259.h"
#include "lib.h"
#include "task.h"
#include "scheduler.h"

#define PIT_IRQ					0x0				
#define PIT_CHANNEL_0			0x40
#define MODE_COMMAND_REGISTER	0x43

#define MODE 					0x36
#define PIT_RELOAD_VALUE		0x8BD3

/* Initialise the PIT */
void pit_init();

/* PIT Interrupt Handler */
extern void pit_handler();


#endif

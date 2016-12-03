#ifndef _PIT_H
#define _PIT_H

#include "types.h"
#include "i8259.h"
#include "lib.h"
#include "task.h"
#include "scheduler.h"

#define PIT_IRQ				0x0				

/* Initialise the PIT */
void pit_init();

/* PIT Interrupt Handler */
extern void pit_handler();


#endif

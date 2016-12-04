#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include "types.h"
#include "i8259.h"
#include "lib.h"
#include "task.h"
#include "syscalls.h"

extern volatile int current_task;

int next_task();
void context_switch();

#endif

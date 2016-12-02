#ifndef _TASK_H
#define _TASK_H

#include "x86_desc.h"
#include "i8259.h"
#include "lib.h"
#include "paging.h"
#include "file.h"
#include "rtc.h"
#include "terminal.h"
#include "keyboard.h"
#include "syscalls.h"

#define NUMBER_TERMINALS 	3

#define TERMINAL_ONE_COLOR		0x9F
#define TERMINAL_TWO_COLOR		0x8F
#define TERMINAL_THREE_COLOR	0xAF

#define _132MB					0x8400000
#define _132MB_4KB				0x8401000
#define _132MB_8KB				0x8402000

#define _32MB 					0x2000000
#define _32MB_4KB 				0x2001000
#define _32MB_8KB 				0x2002000

typedef struct tasks {
	PCB_t* current_process;
	uint8_t keyboard_buffer[BUFFER_SIZE];
	uint32_t virtual_video_mem;
	uint32_t physical_video_mem;
	uint32_t cursor_x;
	uint32_t cursor_y;
	uint32_t esp;
	uint32_t ebp;
	uint32_t eflags;
} tasks_t;

extern int active_terminal;
extern tasks_t terminals[NUMBER_TERMINALS];

void init_tasks();
int switch_tasks(uint32_t index);

#endif

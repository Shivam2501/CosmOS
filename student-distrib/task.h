#define NUMBER_TERMINALS 		3

#ifndef _TASK_H
#define _TASK_H

#include "syscalls.h"
#include "lib.h"
#include "keyboard.h"

#define TERMINAL_ONE_COLOR		0x9F
#define TERMINAL_TWO_COLOR		0x7F
#define TERMINAL_THREE_COLOR	0xAF

#define _132MB_4KB				0x8401000
#define _132MB_8KB				0x8402000
#define _132MB_12KB				0x8403000

#define _32MB 					0x2000000
#define _32MB_4KB 				0x2001000
#define _32MB_8KB 				0x2002000

#define VIDEO_MEM 				0xB8000

#define NUM_COLS 				80
#define NUM_ROWS 				25

#define _4KB 					4096

typedef struct tasks{
	PCB_t* current_process;
	uint8_t keyboard_buffer[BUFFER_SIZE];
	int32_t buffer_index;
	volatile int8_t terminal_read_ready; 
	uint8_t color;
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

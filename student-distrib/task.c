#include "task.h"

int active_terminal;
tasks_t terminals[NUMBER_TERMINALS];

void clear_video_mem(uint32_t video_mem, uint8_t color) {
	int i;
	for(i=0; i<NUM_ROWS*NUM_COLS; i++) {
        *(uint8_t *)(video_mem + (i << 1)) = ' ';
        *(uint8_t *)(video_mem + (i << 1) + 1) = color;
    }
}

/*
 * init_tasks
 *   DESCRIPTION: Initialize all the terminals
 *   INPUTS: none
 *   OUTPUTS: execute shell 1
 *   RETURN VALUE: none
 */ 
void init_tasks() {
	int i;
	//initialize all the terminals to default value
	for(i = 0; i < NUMBER_TERMINALS; i++) {
		//no process running initially
		terminals[i].current_process = NULL;
		//empty keyboard buffer
		memset(terminals[i].keyboard_buffer, '\0', BUFFER_SIZE);
		terminals[i].buffer_index = 0;

		//set cursor x and y to (0,0)
		terminals[i].cursor_x = 0;
		terminals[i].cursor_y = 0;
		//set esp, ebp and eflags to 0
		terminals[i].esp = 0;
		terminals[i].ebp = 0;
		terminals[i].eflags = 0;
	}

	//create video mem for each terminal
	terminals[0].virtual_video_mem = _132MB_4KB;
	terminals[0].physical_video_mem = _32MB;
	add_paging_4kb(_132MB_4KB, _32MB, 0);
	clear_video_mem(_132MB_4KB, TERMINAL_ONE_COLOR);

	terminals[1].virtual_video_mem = _132MB_8KB;
	terminals[1].physical_video_mem = _32MB_4KB;
	add_paging_4kb(_132MB_8KB, _32MB_4KB, 0);
	clear_video_mem(_132MB_8KB, TERMINAL_TWO_COLOR);

	terminals[2].virtual_video_mem = _132MB_12KB;
	terminals[2].physical_video_mem = _32MB_8KB;
	add_paging_4kb(_132MB_12KB, _32MB_8KB, 0);
	clear_video_mem(_132MB_12KB, TERMINAL_THREE_COLOR);

	//start shell 0
	active_terminal = 0;
	update_screen_coord(terminals[active_terminal].cursor_x, terminals[active_terminal].cursor_y);
	update_cursor();
	memcpy((uint8_t*)VIDEO_MEM, (uint8_t *)terminals[active_terminal].virtual_video_mem, _4KB);
	syscall_execute((uint8_t*)"shell");
}

/*
 * switch_tasks
 *   DESCRIPTION: Switch between terminals
 *   INPUTS: Index of the terminal to switch to
 *   OUTPUTS: new terminal is active now
 *   RETURN VALUE: 0 on success, -1 on failure
 */ 
int switch_tasks(uint32_t index) {
	//is the current index is already running
	if(index == active_terminal)
		return 0;

	//check if its a valid index
	if(index >= NUMBER_TERMINALS && index < 0)
		return -1;

	if(terminals[active_terminal].current_process == NULL) {
		if(get_available_pid() == -1)
			return -1;
	}

	cli();

	//inline assembly - save current context
	asm volatile("                  \n\
			movl    %%esp, %0   	\n\
			movl 	%%ebp, %1	    \n\
			pushfl					\n\
			popl	%2				\n\
			"
			: "=S"(terminals[active_terminal].esp), "=b"(terminals[active_terminal].ebp), "=c"(terminals[active_terminal].eflags)
			:
			: "memory", "cc"
			);

	//save the old screen coordinates in the task struct
	terminals[active_terminal].cursor_x = screen_x;
	terminals[active_terminal].cursor_y = screen_y;
	memcpy((uint8_t *)terminals[active_terminal].virtual_video_mem, (uint8_t*)VIDEO_MEM, _4KB);

	//change the active terminal to the current task
	active_terminal = index;
	update_screen_coord(terminals[active_terminal].cursor_x, terminals[active_terminal].cursor_y);
	update_cursor();
	memcpy((uint8_t*)VIDEO_MEM, (uint8_t *)terminals[active_terminal].virtual_video_mem, _4KB);

	//if there is a process running then do a context switch
	if(terminals[active_terminal].current_process != NULL) {
		//context switch
		asm volatile("                  \n\
				movl    %0, %%esp   	\n\
				movl 	%1, %%ebp 	    \n\
				push    %2				\n\
				popfl					\n\
				"
				:
				: "S"(terminals[active_terminal].esp), "b"(terminals[active_terminal].ebp), "c"(terminals[active_terminal].eflags)
				: "memory", "cc"
				);
	}

	sti();

	//check if shell 1 or 2 not executed
	if(terminals[active_terminal].current_process == NULL) {
		syscall_execute((uint8_t*)"shell");
	}
	return 0;
}

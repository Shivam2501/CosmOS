#include "task.h"

//keep track of which terminal user is on and information per terminal
int volatile active_terminal;
tasks_t terminals[NUMBER_TERMINALS];

/*
 * clear_video_mem
 *   DESCRIPTION: Clear the memory passed
 *   INPUTS: video memory to clear and background color to fill in
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */ 
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
	current_task = 0;
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

		terminals[i].terminal_read_ready = 0;
		terminals[i].rtc_interrupt_flag = 0;
	}

	//create video mem for each terminal
	terminals[0].virtual_video_mem = _132MB_4KB;
	terminals[0].physical_video_mem = _32MB;
	terminals[0].color = TERMINAL_ONE_COLOR;
	add_paging_4kb(_132MB_4KB, _32MB);
	clear_video_mem(_132MB_4KB, TERMINAL_ONE_COLOR);

	terminals[1].virtual_video_mem = _132MB_8KB;
	terminals[1].physical_video_mem = _32MB_4KB;
	terminals[1].color = TERMINAL_TWO_COLOR;
	add_paging_4kb(_132MB_8KB, _32MB_4KB);
	clear_video_mem(_132MB_8KB, TERMINAL_TWO_COLOR);

	terminals[2].virtual_video_mem = _132MB_12KB;
	terminals[2].physical_video_mem = _32MB_8KB;
	terminals[2].color = TERMINAL_THREE_COLOR;
	add_paging_4kb(_132MB_12KB, _32MB_8KB);
	clear_video_mem(_132MB_12KB, TERMINAL_THREE_COLOR);

	active_terminal = 0;
	update_screen_coord(terminals[active_terminal].cursor_x, terminals[active_terminal].cursor_y);
	update_cursor();
	memcpy((uint8_t*)VIDEO_MEM, (uint8_t*)terminals[active_terminal].virtual_video_mem, _4KB);
	add_paging_4kb(terminals[active_terminal].virtual_video_mem, VIDEO_MEM);
	sti();
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
	cli();
	
	//if the current index is already running
	if(index == active_terminal)
		return 0;

	//check if its a valid index
	if(index >= NUMBER_TERMINALS && index < 0)
		return -1;

	if(terminals[index].current_process == NULL) {
		if(get_available_pid() == -1)
			return -1;
	}

	//save the old screen coordinates in the task struct
	terminals[active_terminal].cursor_x = screen_x;
	terminals[active_terminal].cursor_y = screen_y;
	add_paging_4kb(terminals[active_terminal].virtual_video_mem, terminals[active_terminal].physical_video_mem);
	memcpy((uint8_t *)terminals[active_terminal].virtual_video_mem, (uint8_t*)VIDEO_MEM, _4KB);

	//change the active terminal to the current task
	active_terminal = index;
	update_screen_coord(terminals[active_terminal].cursor_x, terminals[active_terminal].cursor_y);
	update_cursor();
	memcpy((uint8_t*)VIDEO_MEM, (uint8_t *)terminals[active_terminal].virtual_video_mem, _4KB);
	add_paging_4kb(terminals[active_terminal].virtual_video_mem, VIDEO_MEM);

	//check if shell 1 or 2 not executed
	if(terminals[active_terminal].current_process == NULL) {
		if(current_task == 0) {
			asm volatile("              \n\
				movl    %%esp, %0   	\n\
				movl 	%%ebp, %1	    \n\
				pushfl					\n\
				popl	%2				\n\
				"
				: "=S"(terminals[0].esp), "=b"(terminals[0].ebp), "=c"(terminals[0].eflags)
				:
				: "memory", "cc"
				);
		} else if (current_task == 1) {
			asm volatile("              \n\
				movl    %%esp, %0   	\n\
				movl 	%%ebp, %1	    \n\
				pushfl					\n\
				popl	%2				\n\
				"
				: "=S"(terminals[1].esp), "=b"(terminals[1].ebp), "=c"(terminals[1].eflags)
				:
				: "memory", "cc"
				);
		} else if (current_task == 2) {
			asm volatile("              \n\
				movl    %%esp, %0   	\n\
				movl 	%%ebp, %1	    \n\
				pushfl					\n\
				popl	%2				\n\
				"
				: "=S"(terminals[2].esp), "=b"(terminals[2].ebp), "=c"(terminals[2].eflags)
				:
				: "memory", "cc"
				);
		}
		current_task = active_terminal;
		sti();
		syscall_execute((uint8_t*)"shell");
	}
	sti();
	return 0;
}

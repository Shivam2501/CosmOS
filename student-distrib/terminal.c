#include "terminal.h"

/*
 * Start of System Calls
 */ 

/*
 * terminal_open
 *   DESCRIPTION: Initializes the terminal
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success
 */ 
int32_t terminal_open(void) {
	keyboard_init();
	return 0; 
}

/*
 * terminal_read
 *   DESCRIPTION: Read the input from the terminal
 *   INPUTS: int32_t fd, uint8_t* buf, int32_t nbytes
 *   OUTPUTS: none
 *   RETURN VALUE: bytes read on success, -1 on failure
 */ 
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes) {
	int32_t i;

	//wait until enter is pressed
	while(!get_terminal_status());

	uint8_t *temp_buf = (uint8_t*)buf;
	//copy the terminal line buffer
	for(i = 0; i < nbytes; i++) {
		temp_buf[i] = buffer[i];
		if(i==buffer_index-1)
			break;
	}
	
	//line feed character at the end of the buffer
	if(i < nbytes && temp_buf[i] != '\n')
		temp_buf[++i] = '\n';

	clear_buffer();

	return i+1;
}

/*
 * terminal_write
 *   DESCRIPTION: Write the input on the terminal
 *   INPUTS: int32_t fd, uint8_t* buf, int32_t nbytes
 *   OUTPUTS: none
 *   RETURN VALUE: bytes written on success, -1 on failure
 */ 
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes) {
	int32_t i;
	uint8_t *temp_buf = (uint8_t*)buf;
	
	//disable the keyboard interrupt
	disable_irq(KEYBOARD_IRQ);

	//print the buffer on the screen
	for(i = 0; i < nbytes; i++) {
		putc(temp_buf[i]);
	}

	//enable the keyboard interrupt
	enable_irq(KEYBOARD_IRQ);

	return i+1;
}

/*
 * terminal_close
 *   DESCRIPTION: Close the terminal
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success
 */
int32_t terminal_close(void) {
	clear_buffer();
	return 0;
}

/*
 * End of System Calls
 */ 
 
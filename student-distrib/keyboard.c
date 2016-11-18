#include "keyboard.h"

/*
0 bit - CAPS ON/OFF
1 bit - SHIFT ON/OFF
2 bit - CTRL ON/OFF
*/
uint8_t status;

//buffer to store the keryboard input
uint8_t buffer[BUFFER_SIZE];
int32_t buffer_index;
volatile int terminal_read_ready;

/*
map[0] : caps lock and shift are not pressed
map[1] : shift is pressed
map[2] : caps lock is on
map[3] : caps lock and shift are on
*/
static uint8_t map[MAP_SIZE][KEYCODES_COUNT] = {
	{ 
		0, 0, 
		'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 
		'-', '=', 0, 0,
	 	'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', 
	 	'[', ']', '\n', 0, 
	 	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l' , 
	 	';', '\'', '`', 0, '\\', 
	 	'z', 'x', 'c', 'v', 'b', 'n', 'm',
	 	',', '.', '/', 0, '*', 0, ' ', 0
	},
	{ 
		0, 0, 
		'!', '@', '#', '$', '%', '^', '&', '*', '(', ')', 
		'_', '+', 0, 0,
	 	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', 
	 	'{', '}', '\n', 0, 
	 	'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L' , 
	 	':', '"', '~', 0, '|', 
	 	'Z', 'X', 'C', 'V', 'B', 'N', 'M', 
	 	'<', '>', '?', 0, '*', 0, ' ', 0
	},
	{ 
		0, 0, 
		'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', 
		'-', '=', 0, 0,
	 	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', 
	 	'[', ']', '\n', 0, 
	 	'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L' , 
	 	';', '\'', '`', 0, '\\', 
	 	'Z', 'X', 'C', 'V', 'B', 'N', 'M', 
	 	',', '.', '/', 0, '*', 0, ' ', 0
	},
	{ 
		0, 0, 
		'!', '@', '#', '$', '%', '^', '&', '*', '(', ')', 
		'_', '+', 0, 0,
	 	'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', 
	 	'{', '}', '\n', 0, 
	 	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l' , 
	 	':', '"', '~', 0, '|', 
	 	'z', 'x', 'c', 'v', 'b', 'n', 'm', 
	 	'<', '>', '?', 0, '*', 0, ' ', 0
	}
};

/*
 * process_code
 *   DESCRIPTION: Process the scancode received from the Keyboard
 *   INPUTS: scancode
 *   OUTPUTS: Print the character on the screen
 *   RETURN VALUE: none
 */ 
void process_code(uint8_t scancode) {

	/* Check if key was released 
	Check MSB(0x80) indicating that key
	is being released.
	*/

	if(scancode & CAPSLOCK_BIT) {
		//if shift is released
		if(scancode == RIGHT_SHIFT_LOCK_RELEASED || scancode == LEFT_SHIFT_LOCK_RELEASED)
			toggle_shift();
		//if ctrl is released
		else if(scancode == CTRL_LOCK_RELEASED)
			toggle_ctrl();
	} else {
		//if caps is toggled
		if(scancode == CAPS_LOCK_PRESSED)
			toggle_capslock();
		//if shift is pressed
		else if(scancode == RIGHT_SHIFT_LOCK_PRESSED || scancode == LEFT_SHIFT_LOCK_PRESSED)
			toggle_shift();
		//if ctrl is pressed
		else if(scancode == CTRL_LOCK_PRESSED)
			toggle_ctrl();
		else {

			if(scancode > KEYCODES_COUNT)
				return;

			//enter is pressed
			if(scancode == SCANCODE_ENTER) {
				newline();
				terminal_read_ready = 1;
				//clear_buffer(); //remove during test
				return;
			}

			// if CTRL+L then clear the screen
			if (((status & CTRL_ON)>>2) == 1 && scancode == SCANCODE_L) {
				clear_buffer();
				clear();
				return;
			} 

			// if CTRL+C then notify shell
			if (((status & CTRL_ON)>>2) == 1 && scancode == SCANCODE_C) {
				stop = 1;
				return;
			} 

			//backspace is pressed
			if(scancode == SCANCODE_BACKSPACE) {
				if(buffer_index > 0) {
					buffer[buffer_index-1] = ' ';
					buffer_index--;
					update_coordinate();
					putc(buffer[buffer_index]);
					update_coordinate();
				}
				return;
			}

			//if buff size is reached
			if(buffer_index >= BUFFER_SIZE) 
				return;

			//check if both shift and caps lock are on
			if(((status & SHIFT_ON)>>1) == 1 && (status & CAPSLOCK_ON) == 1) {
				buffer[buffer_index] = map[MAP_SIZE-1][scancode];
				putc(buffer[buffer_index]);
				buffer_index++;
			//check if only shift is pressed
			} else if((status & SHIFT_ON)>>1) {
				buffer[buffer_index] = map[MAP_SIZE-3][scancode];
				putc(buffer[buffer_index]);
				buffer_index++;
			//check if caps lock is on
			} else if (status & CAPSLOCK_ON) {
				buffer[buffer_index] = map[MAP_SIZE-2][scancode];
				putc(buffer[buffer_index]);
				buffer_index++;
			//if both caps lock and shift is not on
			} else{
				buffer[buffer_index] = map[MAP_SIZE-4][scancode];
				putc(buffer[buffer_index]);
				buffer_index++;
			}

		}

	}
}

/*
 * toggle_capslock
 *   DESCRIPTION: Flip the CAPS LOCK bit
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */ 
void toggle_capslock() {
	//toggle the LSB
	status = status^CAPSLOCK_ON;
}

/*
 * toggle_shift
 *   DESCRIPTION: Flip the SHIFT bit
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */ 
void toggle_shift() {
	//toggle the second LSB
	status = status^SHIFT_ON;
}

/*
 * toggle_ctrl
 *   DESCRIPTION: Flip the CTRL bit
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */ 
void toggle_ctrl() {
	//toggle the third LSB
	status = status^CTRL_ON;
}

/*
 * keyboard_handler
 *   DESCRIPTION: Keyboard interrupt handler
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */
void keyboard_handler() {
	uint8_t scancode;

	/* Mask all interrupts 
	cli(); */
	
	/* Read the value in data port */
	scancode = inb(KEYBOARD_DATA_PORT);

	/* Print the character to console */
	process_code(scancode);

	/* Send the End of Interrupt Signal */
	send_eoi(KEYBOARD_IRQ);

	/* Unmask all interrupts 
	sti(); */
}

/*
 * clear_buffer
 *   DESCRIPTION: clear the buffer
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */
void clear_buffer() {
	int32_t i;
	for(i=0; i < BUFFER_SIZE; i++) 
		buffer[i] = '\0';
	buffer_index = 0;
	terminal_read_ready = 0;
}

/*
 * keyboard_init
 *   DESCRIPTION: Keyboard initialisation
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */
void keyboard_init() {
	/* Set the status to 0 initially */
	status = 0x00;
	clear_buffer();

	/* Enable the IRQ Port for Keyboard*/
	enable_irq(KEYBOARD_IRQ);
}

/*
 * get_terminal_status
 *   DESCRIPTION: Return the status of the terminal
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */
int get_terminal_status() {
	return terminal_read_ready;
}

#include "keyboard.h"
#include "i8259.h"
#include "lib.h"

/*
0 bit - CAPS ON/OFF
1 bit - SHIFT ON/OFF
2 bit - CTRL ON/OFF
*/
uint8_t status;

/*
map[0] : caps lock and shift are not pressed
map[1] : shift is pressed
map[2] : caps lock is on
map[3] : caps lock and shift are on
*/
static uint8_t map[4][KEYCODES_COUNT] = {
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
	if(scancode & 0x80) {
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

			//check if both shift and caps lock are on
			if((status & 0x02) == 1 && (status & 0x01) == 1) {
				putc(map[3][scancode]);
			//check if shift is pressed
			} else if(status & 0x02) {
				putc(map[1][scancode]);
			//check if caps lock is on
			} else if (status & 0x01) {
				putc(map[2][scancode]);
			//if both caps lock and shift is not on
			} else {
				putc(map[0][scancode]);
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
	status = status^0x01;
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
	status = status^0x02;
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
	status = status^0x04;
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
 * keyboard_init
 *   DESCRIPTION: Keyboard initialisation
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */
void keyboard_init() {
	/* Set the status to 0 initially */
	status = 0x00;

	/* Enable the IRQ Port for Keyboard*/
	enable_irq(KEYBOARD_IRQ);
}

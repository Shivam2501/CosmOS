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
	 	'[', ']', 0, 0, 
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
	 	'{', '}', 0, 0, 
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
	 	'[', ']', 0, 0, 
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
	 	'{', '}', 0, 0, 
	 	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l' , 
	 	':', '"', '~', 0, '|', 
	 	'z', 'x', 'c', 'v', 'b', 'n', 'm', 
	 	'<', '>', '?', 0, '*', 0, ' ', 0
	}
}

void process_code(uint8_t scancode) {
	/* Check if key was released */
	if(scancode & 0x80) {
		if(scancode == CAPS_LOCK_RELEASED)
			toggle_capslock();
		else if(scancode == RIGHT_SHIFT_LOCK_RELEASED || scancode == LEFT_SHIFT_LOCK_RELEASED)
			toggle_shift();
		else if(scancode == CTRL_LOCK_RELEASED)
			toggle_ctrl();
	} else {

		if(scancode == CAPS_LOCK_PRESSED)
			toggle_capslock();
		else if(scancode == RIGHT_SHIFT_LOCK_PRESSED || scancode == LEFT_SHIFT_LOCK_PRESSED)
			toggle_shift();
		else if(scancode == CTRL_LOCK_PRESSED)
			toggle_ctrl();
		else {

			if(scancode > KEYCODES_COUNT)
				return;

			//check if both shift and caps lock are on
			if((status & 0x02 == 1) & (status & 0x01 == 1)) {
				putc(map[3][scancode])
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

void toggle_capslock() {
	//toggle the LSB
	status = status^0x01;
}

void toggle_shift() {
	//toggle the second LSB
	status = status^0x02;
}

void toggle_ctrl() {
	//toggle the third LSB
	status = status^0x04;
}

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
	asm volatile("iret;");
}

void keyboard_init() {
	/* Set the status to 0 initially */
	status = 0x00;

	/* Enable the IRQ Port for Keyboard*/
	enable_irq(KEYBOARD_IRQ);
}
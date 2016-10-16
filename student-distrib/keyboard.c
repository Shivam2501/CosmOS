#include "keyboard.h"
#include "i8259.h"

/*
0 bit - CAPS ON/OFF
1 bit - SHIFT ON/OFF
2 bit - CTRL ON/OFF
*/
uint8_t status = 0x00;


void process_code(uint8_t scancode) {
	/* Check if key was released */
	if(scancode & 0x80) {

	} else {

	}
}

void keyboard_handler() {
	uint8_t scancode;

	/* Mask all interrupts */
	cli();
	
	/* Read the value in data port until the 
	buffer is empty */
	do {

		scancode = inb(KEYBOARD_DATA_PORT);
		process_code(scancode);

	} while (inb(KEYBOARD_STATUS_PORT) & STATUS_INPUT_BUFFER)

	/* Send the End of Interrupt Signal */
	send_eoi(KEYBOARD_IRQ);

	/* Unmask all interrupts */
	sti();
}

void keyboard_init() {

	/* Mask all interrupts */
	cli();

	/* Disable the devices. 0xA7 is ignored
	if controller is a single channel device */
	outb(0xAD, KEYBOARD_DATA_PORT);
	outb(0xA7, KEYBOARD_DATA_PORT);

	/* Flush the output buffer */

	/* Enable the devices */
	outb(0xAE, KEYBOARD_DATA_PORT);
	outb(0xA8, KEYBOARD_DATA_PORT);

	/* Enable the IRQ Port for Keyboard*/
	enable_irq(KEYBOARD_IRQ);

	/* Unmask all interrupts */
	sti();
}
#include "mouse.h"

position mouse_coord;

uint8_t left_button_pressed;
uint8_t right_button_pressed;
uint8_t middle_button_pressed;
uint8_t mouse_moved;

/*
 * mouse_wait()
 *   DESCRIPTION: Wait for read/write signal
 *   INPUTS: status: 0 for read and 1 for write 
 *   		timeout: how long to wait for the signal
 *   OUTPUTS: return when specific signal is received
 *   RETURN VALUE: none
 */ 
void mouse_wait(int status) {
	//wait for write
	if(status) {
		//wait for byte 1 to be clear
		while((inb(SIGNAL_PORT) & 0x2) != 0);
	} else {	//wait for read
		//wait for byte 0 to be set
		while((inb(SIGNAL_PORT) & 0x1) == 0);
	}

	return;
}

void write_data(uint8_t data) {
	mouse_wait(1);
	// send everytime before writing to data port
	outb(0xD4, SIGNAL_PORT); 
	mouse_wait(1);
	outb(data, DATA_PORT);
	wait_acknowledgement();
}

uint8_t read_data_mouse() {
	mouse_wait(0);
	return inb(DATA_PORT);
}

void wait_acknowledgement() {
	uint8_t ack;
	while(1) {
		ack = read_data_mouse();
		// 0xFA is sent, but after reset 0xAA is sent
		if(ack == 0xFA || ack == 0xAA)
			break;
	}
	return;
}

/*
 * mouse_init()
 *   DESCRIPTION: Initialize the mouse
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */ 
void mouse_init() {
	//reset mouse
	write_data(0xFF);
	
	// Get Compaq Status Byte (0x20)
	mouse_wait(1);
	outb(0x20, SIGNAL_PORT);

	uint8_t status;
	//read the status
	status = read_data_mouse();
	//set bit 1 (0x2) and clear bit 5 (0xEF)
	status = (status | 0x2) & (0xEF);

	// Set Compaq Status Byte (0x60)
	mouse_wait(1);
	outb(0x60, SIGNAL_PORT);
	//write status to data port
	mouse_wait(1);
	outb(status, DATA_PORT);

	//enable auxilary device (0xA8)
	mouse_wait(1);
	outb(0xA8, SIGNAL_PORT);

	//enable acknowledgment (0xF4)
	write_data(0xF4);

	//initialize all the values
	//mouse_coord.position_x = curr_desktop->context->width / 2;
	//mouse_coord.position_y = curr_desktop->context->height / 2;
	left_button_pressed = 0;
	right_button_pressed = 0;
	middle_button_pressed = 0;
	mouse_moved = 0;

	/* Enable the IRQ Port for Mouse*/
	enable_irq(MOUSE_IRQ);
}

/*
 * mouse_handler()
 *   DESCRIPTION: Handles the mouse movement
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */
void mouse_handler() {
	//check interrupt from mouse(0x20)
	if(((inb(SIGNAL_PORT) & 0x1) != 0) && ((inb(SIGNAL_PORT) & 0x20) != 0)) {
		uint8_t byte1 = inb(DATA_PORT);
		//check for overflow bits (0x80 and 0x40) to be clear
		//check bit 3 to be set to verify package
		if(((byte1 & 0x80) == 0) && ((byte1 & 0x40) == 0) && ((byte1 & 0x8) != 0)) {
			int32_t byte2 = read_data_mouse();
			//check if delta X is a negative number (bit 5/ 0x10)d b
			if(byte1 & 0x10) {
				byte2 |= 0xFFFFFF00;
			}

			int32_t byte3 = read_data_mouse();
			//check if delta y is a negative number (bit 6/ 0x20)
			if(byte1 & 0x20) {
				byte3 |= 0xFFFFFF00;
			}

			//check if left button(bit 1/ 0x1) is pressed
			if(byte1 & 0x1) {
				//if no movement and left button is already pressed
				if(!byte2 && !byte3) {
					if(left_button_pressed) {
						handle_double_click();
					}
				}
				left_button_pressed = 1;
			} else {
				if(left_button_pressed) {
					left_button_pressed = 0;
				}
			}

			//check if right button (bit 2/ 0x2) is pressed
			if(byte1 & 0x2) {
				right_button_pressed = 1;
			} else {
				if(right_button_pressed) {
					right_button_pressed = 0;
				}
			}

			//check if middle button (bit 3/ 0x4) is pressed
			if(byte1 & 0x4) {
				middle_button_pressed = 1;
			} else {
				if(middle_button_pressed) {
					middle_button_pressed = 0;
				}
			}

			//check if mouse moved
			if(byte2 || byte3) {
				mouse_moved = 1;
				handle_mouse_movement(byte2, byte3);
			} else {
				mouse_update(curr_desktop, mouse_coord.position_y, mouse_coord.position_x, left_button_pressed);
				mouse_moved = 0;
			}
		}

	}
	send_eoi(MOUSE_IRQ);
}

void handle_mouse_movement(int32_t delta_x, int32_t delta_y) {
	//mouse_coord.position_x += delta_x;
	//mouse_coord.position_y -= delta_y;
	mouse_update(curr_desktop, delta_x, delta_y, left_button_pressed);
	if(left_button_pressed) {
		handle_drag();
	}
}

void handle_double_click() {

}

void handle_drag() {

}

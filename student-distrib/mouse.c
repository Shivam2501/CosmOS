#include "mouse.h"

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
	//set bit 1 and clear bit 5
	status |= 0x2;
	status &= ~(0x20);

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
}

void mouse_handler() {

}

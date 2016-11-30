#ifndef MOUSE_H
#define MOUSE_H

#include "types.h"
#include "i8259.h"
#include "lib.h"

#define MOUSE_IRQ 		0x2C
#define SIGNAL_PORT		0x64
#define DATA_PORT		0x60

typedef struct mouse_location {
	int32_t position_x;
	int32_t position_y;
} position;

/* Mouse Handler */
extern void mouse_handler();

void mouse_wait(int status);
void mouse_init();

/* Helper functions */
void write_data(uint8_t data);
uint8_t read_data_mouse();
void wait_acknowledgement();

#endif

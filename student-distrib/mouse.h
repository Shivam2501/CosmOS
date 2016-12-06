#ifndef MOUSE_H
#define MOUSE_H

#include "types.h"
#include "i8259.h"
#include "lib.h"
#include "windowing.h"

#define MOUSE_IRQ 		12
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

/* Handle mouse actions */
void handle_mouse_movement(int32_t delta_x, int32_t delta_y);
void handle_drag();
void handle_double_click();

#endif

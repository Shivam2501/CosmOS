/* keyboard.h - Defines for keyboard handler
 * vim:ts=4 noexpandtab
 */

#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "types.h"
#include "i8259.h"
#include "lib.h"
#include "test.h"
#include "syscalls.h"

#define KEYBOARD_IRQ					0x01
#define KEYBOARD_DATA_PORT				0x60
#define KEYBOARD_STATUS_PORT			0x64

/* Scan code for Keys */
#define CAPS_LOCK_PRESSED				0x3A
#define CAPS_LOCK_RELEASED				0xBA

#define RIGHT_SHIFT_LOCK_PRESSED		0x36
#define RIGHT_SHIFT_LOCK_RELEASED		0xB6

#define LEFT_SHIFT_LOCK_PRESSED			0x2A
#define LEFT_SHIFT_LOCK_RELEASED		0xAA

#define CTRL_LOCK_PRESSED				0x1D
#define CTRL_LOCK_RELEASED				0x9D

#define KEYCODES_COUNT					0x3C

#define CAPSLOCK_BIT					0x80

#define MAP_SIZE						0x4

#define CAPSLOCK_ON						0x01
#define SHIFT_ON						0x02
#define CTRL_ON							0x04

#define SCANCODE_L						0x26
#define SCANCODE_C 						0x2E
#define SCANCODE_ENTER					0x1C
#define SCANCODE_BACKSPACE				0x0E

#define BUFFER_SIZE						0x80

/* Initialise the Keyboard */
void keyboard_init();

/* Toogle the Capslock */
void toggle_capslock();

/* Toogle the Shift */
void toggle_shift();

/* Toogle the Ctrl */
void toggle_ctrl();

//buffer to store the keryboard input
extern uint8_t buffer[BUFFER_SIZE];
extern int32_t buffer_index;
extern volatile int ctrl_c_ready;

/* Keyboard Interrupt Handler */
extern void keyboard_handler();

/* Scan the code and Echo the string */
void process_code(uint8_t scancode);

/* Clear the buffer */
void clear_buffer();

int get_terminal_status();

#endif /* _KEYBOARD_H */

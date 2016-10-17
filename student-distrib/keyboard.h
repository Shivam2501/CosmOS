/* keyboard.h - Defines for keyboard handler
 * vim:ts=4 noexpandtab
 */

#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "types.h"

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

/* Initialise the Keyboard */
void keyboard_init();

/* Toogle the Capslock */
void toggle_capslock();

/* Toogle the Shift */
void toggle_shift();

/* Toogle the Ctrl */
void toggle_ctrl();

/* Keyboard Interrupt Handler */
extern void keyboard_handler();

/* Scan the code and Echo the string */
void process_code(uint8_t scancode);

#endif /* _KEYBOARD_H */

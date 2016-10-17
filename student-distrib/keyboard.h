/* keyboard.h - Defines for keyboard handler
 * vim:ts=4 noexpandtab
 */

#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#define KEYBOARD_IRQ					0x01
#define KEYBOARD_DATA_PORT				0x60
#define KEYBOARD_STATUS_PORT			0x64

/* #define STATUS_INPUT_BUFFER				0x02 */

/* Scan code for Keys */
#define CAPS_LOCK_PRESSED				0x3A
#define CAPS_LOCK_RELEASED				0xBA

#define RIGHT_SHIFT_LOCK_PRESSED		0x36
#define RIGHT_LOCK_RELEASED				0xB6

#define LEFT_SHIFT_LOCK_PRESSED			0x2A
#define LEFT_SHIFT_LOCK_RELEASED		0xAA

#define CTRL_LOCK_PRESSED				0x1D
#define CTRL_LOCK_RELEASED				0x9D

#define KEYCODES_COUNT					0x3C

void keyboard_init();

void keyboard_handler();

void process_code(uint8_t scancode);

#endif /* _KEYBOARD_H */

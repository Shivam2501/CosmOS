/* keyboard.h - Defines for keyboard handler
 * vim:ts=4 noexpandtab
 */

#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#define KEYBOARD_IRQ			0x01
#define KEYBOARD_DATA_PORT		0x60
#define KEYBOARD_STATUS_PORT	0x64

#define STATUS_INPUT_BUFFER		0x02

void keyboard_init();

void keyboard_handler();

void process_code(uint8_t scancode);

#endif /* _KEYBOARD_H */

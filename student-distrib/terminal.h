/* terminal.h - Defines for keyboard handler
 * vim:ts=4 noexpandtab
 */

#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "types.h"
#include "keyboard.h"
#include "i8259.h"
#include "lib.h"

#define KEYBOARD_IRQ					0x01

/* System calls for terminal driver */
int32_t terminal_open(void);
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes);
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t terminal_close(void);

#endif /* _TERMINAL_H */

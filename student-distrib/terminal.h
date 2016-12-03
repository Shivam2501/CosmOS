/* terminal.h - Defines for keyboard handler
 * vim:ts=4 noexpandtab
 */

#ifndef _TERMINAL_H
#define _TERMINAL_H

#include "types.h"
#include "keyboard.h"
#include "i8259.h"
#include "lib.h"
#include "scheduler.h"

#define KEYBOARD_IRQ					0x01

int32_t terminal_open(const uint8_t* filename);
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes);
int32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t terminal_close(int32_t fd);

#endif /* _TERMINAL_H */

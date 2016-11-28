#ifndef WINDOW_H
#define WINDOW_H

#include "lib.h"
#include "modex.h"
#include "memory_allocator.h"

typedef struct window {
	uint16_t x;
	uint16_t y;
	uint16_t width;
	uint16_t height;
} window;

window* new_window(uint16_t x, uint16_t y, uint16_t width, uint16_t height);

#endif

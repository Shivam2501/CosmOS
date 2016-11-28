#ifndef WINDOW_H
#define WINDOW_H

#include "lib.h"
#include "modex.h"
#include "memory_allocator.h"

typedef struct context {
	uint8_t* buffer;
	uint32_t width;
	uint32_t height;
} context;

typedef struct window {
	uint32_t x;
	uint32_t y;
	uint32_t width;
	uint32_t height;
	context* context;
} window;

typedef struct ListNode_struct {
	void* payload;
	struct ListNode_struct* prev;
	struct ListNode_Struct* next;
} ListNode;

typedef struct List {
	uint32_t count;
	ListNode* root;
} List;

typedef struct desktop {
	List* children; 
	context* context;
} desktop;

window* new_window(uint32_t x, uint32_t y, uint32_t width, uint32_t height, context* context);
void draw_rectangle(context* context, uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color);
void window_paint(window* window);

#endif

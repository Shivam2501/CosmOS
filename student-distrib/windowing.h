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
	uint32_t color;
} window;

typedef struct ListNode_struct {
	void* payload;
	struct ListNode_struct* prev;
	struct ListNode_struct* next;
} ListNode;

typedef struct List {
	uint32_t count;
	ListNode* root;
} List;

typedef struct desktop {
	List* children; 
	context* context;
	uint8_t left_button_state;
	uint16_t mouse_x;
	uint16_t mouse_y;

	window* drag_window;
	uint16_t drag_offset_x;
	uint16_t drag_offset_y;
} desktop;

window* new_window(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color, context* context);
void draw_rectangle(context* context, uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color);
void window_paint(window* window);

/* List Functions */
List* new_list();
ListNode* new_ListNode(void* payload);
int add_to_list(List* curr_list, void* payload);
void* find_node(List* curr_list, uint32_t index);
void* delete_node(List* curr_list, uint32_t index);

desktop* new_desktop(context* cont);
window* new_window_desktop(desktop* curr_desktop, uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color);
void desktop_paint(desktop* curr_desktop);
void clear_desktop(desktop* curr_desktop);

void mouse_update(desktop* curr_desktop, uint16_t mouse_x, uint16_t mouse_y, uint8_t button);


#endif

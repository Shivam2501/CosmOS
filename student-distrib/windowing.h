#ifndef WINDOW_H
#define WINDOW_H

#include "lib.h"
#include "modex.h"
#include "memory_allocator.h"

#define WINDOW_BACKGROUND	17
#define WINDOW_TITLE		24
#define WINDOW_BORDER		36

typedef struct rectangle {
	int32_t top;
	int32_t left;
	int32_t bottom;
	int32_t right;
} rectangle;

typedef struct ListNode_struct {
	void* payload;
	struct ListNode_struct* prev;
	struct ListNode_struct* next;
} ListNode;

typedef struct List {
	uint32_t count;
	ListNode* root;
} List;

typedef struct context {
	uint8_t* buffer;
	uint32_t width;
	uint32_t height;
	List* clipped_rectangles;
} context;

typedef struct window {
	uint32_t x;
	uint32_t y;
	uint32_t width;
	uint32_t height;
	context* context;
} window;

typedef struct desktop {
	List* children; 
	context* context;
	uint8_t left_button_state;
	int32_t mouse_x;
	int32_t mouse_y;

	window* drag_window;
	int32_t drag_offset_x;
	int32_t drag_offset_y;
} desktop;

extern desktop* curr_desktop;

void init_desktop();

context* new_context(uint32_t width, uint32_t height, uint8_t* buffer);
void add_rectangle(context* curr_context, rectangle* new_rectangle);
void remove_rectangle(context* curr_context, rectangle* new_rectangle);
void clear_rectangles(context* curr_context);
void context_draw_rectangle(context* cont, uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color);
void draw_clipped_rectangle(context* cont, uint32_t x, uint32_t y, uint32_t width, uint32_t height, 
	rectangle* clipped_rectangle, uint32_t color);
void horiz_line(context* cont, uint32_t x, uint32_t y, uint32_t length, uint32_t color);
void vert_line(context* cont, uint32_t x, uint32_t y, uint32_t length, uint32_t color);

/* Window Functions */
window* new_window(uint32_t x, uint32_t y, uint32_t width, uint32_t height, context* context);
void draw_rectangle(context* context, uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color);
void window_paint(window* window);
List* overlapping_windows(desktop* curr_desktop, window* window);

/* Rectangle Functions */
rectangle* new_rectangle(int32_t top, int32_t left, int32_t bottom, int32_t right);
List* split_rectangle(rectangle* lower_rectangle, rectangle* upper_rectangle);

/* List Functions */
List* new_list();
ListNode* new_ListNode(void* payload);
int add_to_list(List* curr_list, void* payload);
void* find_node(List* curr_list, uint32_t index);
void* delete_node(List* curr_list, uint32_t index);

/* Desktop Functions */
desktop* new_desktop(context* cont);
window* new_window_desktop(desktop* curr_desktop, uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color);
void desktop_paint(desktop* curr_desktop);
void clear_desktop(desktop* curr_desktop);

void mouse_update(desktop* curr_desktop, int32_t mouse_x, int32_t mouse_y, uint8_t button);


#endif

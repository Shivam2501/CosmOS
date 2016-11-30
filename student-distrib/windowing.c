#include "windowing.h"

/*
 * new_window
 *   DESCRIPTION: Create a new window
 *   INPUTS: x, y, width and height
 *   OUTPUTS: none
 *   RETURN VALUE: pointer to the window struct
 */ 
window* new_window(uint32_t x, uint32_t y, uint32_t width, uint32_t height, context* cont) {

	//allocate memory for a new window struct
	window* new_window = (window*)kmalloc(sizeof(window));
	//if malloc fails
	if(new_window == NULL)
		return NULL;	

	//assign values if successful malloc
	new_window->x = x;
	new_window->y = y;
	new_window->width = width;
	new_window->height = height;
	new_window->context = cont;

	return new_window;
}

/*
 * draw_rectangle
 *   DESCRIPTION: Draw a rectangle
 *   INPUTS: context for framebuffer and rectangle coordinates
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */ 
void draw_rectangle(context* cont, uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color) {
	uint32_t current_x;
	uint32_t max_x = x + width;
	uint32_t max_y = y + height;

	//check bounds
	if(max_x > cont->width)
		max_x = cont->width;
	if(max_y > cont->height)
		max_y = cont->height;

	int index, plane, offset;
	for(; y < max_y; y++) {
		for(current_x = x; current_x < max_x; current_x++) {
			index = y * cont->width + current_x;
			plane = current_x % 4;
			offset = (current_x / 4) + (y * cont->width / 4);
			cont->buffer[ ((65536 / 4) * plane) + offset] = color;
		}
	}
}

/*
 * window_paint
 *   DESCRIPTION: Paint the window on the screen
 *   INPUTS: window struct
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */ 
void window_paint(window* curr_window) {
	uint32_t fill_color = 17;
	draw_rectangle(curr_window->context, curr_window->x, curr_window->y, curr_window->width, curr_window->height, fill_color);
}

/*
 * new_list
 *   DESCRIPTION: Create an empty list
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: pointer to the list
 */ 
List* new_list() {

	List* new_list = (List*)kmalloc(sizeof(List));
	//if malloc fails
	if(new_list == NULL)
		return NULL;

	new_list->count = 0;
	new_list->root = NULL;

	return new_list;
}

/*
 * new_ListNode
 *   DESCRIPTION: Create an empty list node
 *   INPUTS: payload
 *   OUTPUTS: none
 *   RETURN VALUE: pointer to the list node
 */
ListNode* new_ListNode(void* payload) {

	ListNode* node = (ListNode*)kmalloc(sizeof(ListNode));

	if(node == NULL)
		return NULL;

	node->prev = NULL;
	node->next = NULL;
	node->payload = payload;

	return node;
}

/*
 * add_to_list
 *   DESCRIPTION: Add a new node to the list
 *   INPUTS: list to be added to and the payload for the node
 *   OUTPUTS: none
 *   RETURN VALUE: 1 on success and 0 on failure
 */
int add_to_list(List* curr_list, void* payload) {

	ListNode* new_node = new_ListNode(payload);

	if(new_node == NULL)
		return 0;

	//check if the list is empty
	if(!curr_list->root) {
		curr_list->root = new_node;
	} else {
		ListNode* curr_node = curr_list->root;

		//go to the end of the curr_list
		while(curr_node->next)
			curr_node = curr_node->next;

		curr_node->next = new_node;
		new_node->prev = curr_node;
	}

	curr_list->count++;
	return 1;
}

/*
 * find_node
 *   DESCRIPTION: Find the node in the given list
 *   INPUTS: list and the index of the node to be found
 *   OUTPUTS: none
 *   RETURN VALUE: payload of the node on success and 0 on failure
 */
void* find_node(List* curr_list, uint32_t index) {
	//check if current list is empty or index is greater than elements in the list
	if(curr_list->count == 0 || curr_list->count < index)
		return (void*)0;

	ListNode* curr_node = curr_list->root;
	while(curr_node != NULL && index > 0) {
		curr_node = curr_node->next;
		index--;
	}
	if(curr_node == NULL)
		return (void*)0;
	else
		return curr_node->payload;
}

/*
 * new_desktop
 *   DESCRIPTION: Initialize a new desktop
 *   INPUTS: context of the framebuffer
 *   OUTPUTS: none
 *   RETURN VALUE: desktop created
 */
desktop* new_desktop(context* cont) {
	desktop* new_desktop = (desktop*)kmalloc(sizeof(desktop));

	if(new_desktop == NULL)
		return NULL;

	new_desktop->children = new_list();
	if(new_desktop->children == NULL) {
		kfree(new_desktop);
		return NULL;
	}

	new_desktop->context = cont;
	return new_desktop;
}

/*
 * new_desktop
 *   DESCRIPTION: Add a window to desktop
 *   INPUTS: desktop and info about the window
 *   OUTPUTS: none
 *   RETURN VALUE: window created
 */
window* new_window_desktop(desktop* curr_desktop, uint32_t x, uint32_t y, uint32_t width, uint32_t height) {
	window* curr_window = new_window(x, y, width, height, curr_desktop->context);

	if(curr_window == NULL)
		return NULL;

	//add the window to the list
	int add_window = add_to_list(curr_desktop->children, (void*)curr_window);
	if(!add_window) {
		kfree(curr_window);
		return NULL;
	} else {
		return curr_window;
	}
}

/*
 * clear_desktop
 *   DESCRIPTION: clear the screen
 *   INPUTS: desktop 
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */
void clear_desktop(desktop* curr_desktop) {
	draw_rectangle(curr_desktop->context, 0, 0, curr_desktop->context->width, curr_desktop->context->height, BACKGROUND_COLOR);
}

/*
 * desktop_paint
 *   DESCRIPTION: Draw all the windows in the desktop list
 *   INPUTS: desktop
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */
void desktop_paint(desktop* curr_desktop) {
	clear_desktop(curr_desktop);

	int i;
	window* curr_window;
	for(i = 0; (curr_window = (window*)find_node(curr_desktop->children, i));i++)
		window_paint(curr_window);
}

#include "windowing.h"

desktop* curr_desktop;

void init_desktop() {
	context* cont = (context*)kmalloc(sizeof(context));
	cont->buffer = textBuffer;
	cont->width = IMAGE_X_DIM;
	cont->height = 200;

	curr_desktop = new_desktop(cont);
}

/*
 * new_window
 *   DESCRIPTION: Create a new window
 *   INPUTS: x, y, width and height
 *   OUTPUTS: none
 *   RETURN VALUE: pointer to the window struct
 */ 
window* new_window(uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color, context* cont) {

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
	new_window->color = color;

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
			plane = current_x % MODEX_PLANES;
			offset = (current_x / MODEX_PLANES) + (y * cont->width / MODEX_PLANES);
			cont->buffer[ ((MODE_X_MEM_SIZE / MODEX_PLANES) * plane) + offset] = color;
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
	draw_rectangle(curr_window->context, curr_window->x, curr_window->y, curr_window->width, curr_window->height, curr_window->color);
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
 * delete_node
 *   DESCRIPTION: Delete a node from the list
 *   INPUTS: list to be deleted from and index of the node
 *   OUTPUTS: none
 *   RETURN VALUE: 1 on success and 0 on failure
 */
void* delete_node(List* curr_list, uint32_t index) {
	//check if current list is empty or index is greater than elements in the list
	if(curr_list->count == 0 || curr_list->count < index)
		return (void*)0;

	ListNode* curr_node = curr_list->root;
	uint32_t curr_index;
	for(curr_index = 0; (curr_index < index) && (curr_node != NULL); curr_index++) {
		curr_node = curr_node->next;
	}
	if(curr_node == NULL)
		return (void*)0;

	void* payload = curr_node->payload;

	if(curr_node->prev != NULL)
		curr_node->prev->next = curr_node->next;

	if(curr_node->next != NULL)
		curr_node->next->prev = curr_node->prev;

	//check if deleting the root node
	if(!index)
		curr_list->root = curr_node->next;
	curr_list->count --;
	//free the memory for the new node
	kfree(curr_node);

	return payload;
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

	//set the default values for mouse
	new_desktop->left_button_state = 0;
	new_desktop->mouse_x = new_desktop->context->width / 2;
	new_desktop->mouse_y = new_desktop->context->height / 2;

	new_desktop->drag_window = (window*)0;
	new_desktop->drag_offset_x = 0;
	new_desktop->drag_offset_y = 0;

	return new_desktop;
}

/*
 * new_desktop
 *   DESCRIPTION: Add a window to desktop
 *   INPUTS: desktop and info about the window
 *   OUTPUTS: none
 *   RETURN VALUE: window created
 */
window* new_window_desktop(desktop* curr_desktop, uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color) {
	window* curr_window = new_window(x, y, width, height, color, curr_desktop->context);

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
 * mouse_update
 *   DESCRIPTION: process mouse event
 *   INPUTS: desktop, mouse coord and button
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */
void mouse_update(desktop* curr_desktop, int32_t mouse_x, int32_t mouse_y, uint8_t button) {

	int i;
	window* curr_window;
	int temp;
	if(mouse_x) {
		temp = mouse_x;
		if(mouse_x < 0)
			temp *= -1;
		curr_desktop->mouse_x += mouse_x / temp;
	}
	if(mouse_y) {
		temp = mouse_y;
		if(mouse_y < 0)
			temp *= -1;
		curr_desktop->mouse_y -= mouse_y / temp;
	}

	//check bounds for x
	if(curr_desktop->mouse_x < 0) {
		curr_desktop->mouse_x = 0;
	} else if(curr_desktop->mouse_x >= curr_desktop->context->width) {
		curr_desktop->mouse_x = curr_desktop->context->width - 1;
	}

	//check bounds for y
	if(curr_desktop->mouse_y < 0) {
		curr_desktop->mouse_y = 0;
	} else if(curr_desktop->mouse_y >= curr_desktop->context->height) {
		curr_desktop->mouse_y = curr_desktop->context->height - 1;
	}

	//check if a button was pressed
	if(button) {
		if(!curr_desktop->left_button_state) {
			//iterate through all the windows
			for(i = curr_desktop->children->count - 1; i >= 0; i--) {
				curr_window = (window*)find_node(curr_desktop->children, i);
				//check if this window was clicked
				if(curr_desktop->mouse_x >= curr_window->x && curr_desktop->mouse_x < (curr_window->x + curr_window->width) &&
					curr_desktop->mouse_y >= curr_window->y && curr_desktop->mouse_y < (curr_window->y + curr_window->height)) {
					//put this window at the head of the list
					delete_node(curr_desktop->children, i);
					add_to_list(curr_desktop->children, (void*)curr_window);

					//offset between mouse and the window being dragged
					curr_desktop->drag_offset_x = curr_desktop->mouse_x - curr_window->x;
					curr_desktop->drag_offset_y = curr_desktop->mouse_y - curr_window->y;
					curr_desktop->drag_window = curr_window;

					break;
				}
			}
		}
	} else {
		curr_desktop->drag_window = (window*)0;
	}

	//check if drag window is there
	if(curr_desktop->drag_window) {
		curr_desktop->drag_window->x = curr_desktop->mouse_x - curr_desktop->drag_offset_x;
		curr_desktop->drag_window->y = curr_desktop->mouse_y - curr_desktop->drag_offset_y;
	}

	//update the screen
	desktop_paint(curr_desktop);
	//update the mouse button state for desktop
	curr_desktop->left_button_state = button;
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

	//draw a rectangle for a mouse
	draw_rectangle(curr_desktop->context, curr_desktop->mouse_x, curr_desktop->mouse_y, 
		2, 2, 40);
	outputBuffer();
}

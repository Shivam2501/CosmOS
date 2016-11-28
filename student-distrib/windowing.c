#include "windowing.h"

/*
 * new_window
 *   DESCRIPTION: Create a new window
 *   INPUTS: x, y, width and height
 *   OUTPUTS: none
 *   RETURN VALUE: pointer to the window struct
 */ 
window* new_window(uint32_t x, uint32_t y, uint32_t width, uint32_t height, context* context) {

	//allocate memory for a new window struct
	window* new_window = (window*)kmalloc(sizeof(window));
	//if malloc fails
	if(new_window == NULL)
		return -1;	

	//assign values if successful malloc
	new_window->x = x;
	new_window->y = y;
	new_window->width = width;
	new_window->height = height;
	new_window->context = context;

	return new_window;
}

/*
 * draw_rectangle
 *   DESCRIPTION: Draw a rectangle
 *   INPUTS: context for framebuffer and rectangle coordinates
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */ 
void draw_rectangle(context* context, uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color) {
	uint32_t current_x;
	uint32_t max_x = x + width;
	uint32_t max_y = y + height;

	//check bounds
	if(max_x > context->width)
		max_x = context->width;
	if(max_y > context->height)
		max_y = context->height;

	for(; y < max_y; y++) {
		for(current_x = x; current_x < max_x; current_x++) {
			context->buffer[y * context->width + current_x] = color;
		}
	}
}

uint8_t pseudo_rand_8() {
	static uint16_t seed = 0;
	return (uint8_t)(seed = (12657*seed + 12345) % 256);
}

/*
 * window_paint
 *   DESCRIPTION: Paint the window on the screen
 *   INPUTS: window struct
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */ 
void window_paint(window* window) {
	uint32_t fill_color = 0xFF000000 | pseudo_rand_8() << 16 | pseudo_rand_8() << 8 | pseudo_rand_8();
	draw_rectangle(window->context, window->x, window->y, window->width, window->height, fill_color);
}

/*
 * new_list
 *   DESCRIPTION: Create an empty list
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: pointer to the list
 */ 
List* new_list() {

	List* list = (List*)kmalloc(sizeof(List));
	//if malloc fails
	if(list == NULL)
		return -1;

	list->count = 0;
	list->root = (ListNode*)0;

	return list;
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
		return -1;

	node->prev = (ListNode*)0;
	node->next = (ListNode*)0;
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
int add_to_list(list* list, void* payload) {

	ListNode* new_node = new_ListNode(payload);

	if(new_node == -1)
		return 0;

	//check if the list is empty
	if(!list->root) {
		list->root = new_node;
	} else {
		ListNode* curr_node = list->root;

		//go to the end of the list
		while(curr_node->next)
			curr_node = curr_node->next;

		curr_node->next = new_node;
		new_node->prev = curr_node;
	}

	list->count++;
	return 1;
}


#include "windowing.h"

desktop* curr_desktop;

void init_desktop() {
	context* cont = new_context(IMAGE_X_DIM, 200, textBuffer);
	curr_desktop = new_desktop(cont);
}

/*
 * new_context
 *   DESCRIPTION: Create a new context
 *   INPUTS: members
 *   OUTPUTS: none
 *   RETURN VALUE: pointer to the context struct
 */ 
context* new_context(uint32_t width, uint32_t height, uint8_t* buffer) {

	//allocate memory for a new window struct
	context* new_context = (context*)kmalloc(sizeof(context));
	//if malloc fails
	if(new_context == NULL)
		return NULL;	

	new_context->clipped_rectangles = new_list();
	if(new_context->clipped_rectangles == NULL) {
		kfree(new_context);
		return (context*)0;
	}

	//assign values if successful malloc
	new_context->width = width;
	new_context->height = height;
	new_context->buffer = buffer;

	return new_context;
}

/*
 * remove_rectangle
 *   DESCRIPTION: Remove a rectangle from the list of clipped rectangles
 *   INPUTS: context and the rectangle to be removed
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */ 
void remove_rectangle(context* curr_context, rectangle* new_rectangle) {
	int i;
	rectangle *curr_rectangle;

	List* curr_list;

	for(i = 0; i < curr_context->clipped_rectangles->count; ) {
		curr_rectangle = find_node(curr_context->clipped_rectangles, i);

		/* check if this rectangle in the list overlaps with the new rectangle.
		stack overflow:
		Cond1. If A's left edge is to the right of the B's right edge, - then A is Totally to right Of B
		Cond2. If A's right edge is to the left of the B's left edge, - then A is Totally to left Of B
		Cond3. If A's top edge is below B's bottom edge, - then A is Totally below B
		Cond4. If A's bottom edge is above B's top edge, - then A is Totally above B
		*/
		if(!(curr_rectangle->left <= new_rectangle->right && 
			curr_rectangle->right >= new_rectangle->left &&
			curr_rectangle->top <= new_rectangle->bottom &&
			curr_rectangle->bottom >= new_rectangle->top)) {
			i++;
			continue;
		}

		delete_node(curr_context->clipped_rectangles, i);

		//split the current rectangle 
		curr_list = split_rectangle(curr_rectangle, new_rectangle);
		kfree(curr_rectangle);

		//copy all the rectangles from the split into the list
		while(curr_list->count) {
			curr_rectangle = (rectangle*)delete_node(curr_list, 0);
			add_to_list(curr_context->clipped_rectangles, curr_rectangle);
		}

		kfree(curr_list);

		i = 0;
	}
}

/*
 * add_rectangle
 *   DESCRIPTION: Add a new rectangle to the list of clipped rectangles
 *   INPUTS: context and the new rectangle to be added
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */ 
void add_rectangle(context* curr_context, rectangle* new_rectangle) {
	remove_rectangle(curr_context, new_rectangle);
	add_to_list(curr_context->clipped_rectangles, new_rectangle);
}

/*
 * clear_rectangles
 *   DESCRIPTION: Clear the list of clipped rectangles
 *   INPUTS: context
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */ 
void clear_rectangles(context* curr_context) {
	rectangle* curr_rectangle;

	while(curr_context->clipped_rectangles->count) {
		curr_rectangle = (rectangle*)delete_node(curr_context->clipped_rectangles, 0);
		kfree(curr_rectangle);
	}
}

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
 * new_rectangle
 *   DESCRIPTION: Construct a rectangle
 *   INPUTS: boundaries
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */ 
rectangle* new_rectangle(int32_t top, int32_t left, int32_t bottom, int32_t right) {
	//allocate memory for a new rectangle struct
	rectangle* new_rect = (rectangle*)kmalloc(sizeof(rectangle));
	//if malloc fails
	if(new_rect == NULL)
		return NULL;	

	//assign values if successful malloc
	new_rect->top = top;
	new_rect->left = left;
	new_rect->bottom = bottom;
	new_rect->right = right;

	return new_rect;
}

/*
 * split_rectangle
 *   DESCRIPTION: Clip a lower rectangle by an upper rectangle
 *   INPUTS: both the rectangle pointers
 *   OUTPUTS: none
 *   RETURN VALUE: list of rectangles
 */ 
List* split_rectangle(rectangle* lower_rectangle, rectangle* upper_rectangle) {
	//list which stores the clipped rectangles
	List* clipped_rectangle = new_list();

	if(clipped_rectangle == NULL)
		return (List*)0;

	rectangle temp_rectangle; //store the rectangle we are going to clip

	temp_rectangle.top = lower_rectangle->top;
	temp_rectangle.left = lower_rectangle->left;
	temp_rectangle.bottom = lower_rectangle->bottom;
	temp_rectangle.right = lower_rectangle->right;

	rectangle *new_rect;

	//split by left edge
	if((upper_rectangle->left <= temp_rectangle.right) && (upper_rectangle->left >= temp_rectangle.left)) {
		/*create a new rectangle from 
		temp_rectangle left, top, bottom
		and upper rectangle's left boundary.
		*/
		new_rect = new_rectangle(temp_rectangle.top, temp_rectangle.left, temp_rectangle.bottom, 
			upper_rectangle->left - 1);

		//sanity check
		if(new_rect == NULL) {
			kfree(clipped_rectangle);
			return (List*)0;
		}

		//if successfully cut then add it to the list of clipped rectangles
		add_to_list(clipped_rectangle, new_rect);
		temp_rectangle.left = upper_rectangle->left;
	} 

	//split by top edge
	if((upper_rectangle->top <= temp_rectangle.bottom) && (upper_rectangle->top >= temp_rectangle.top)) {
		/*create a new rectangle from 
		temp_rectangle left, top, right
		and upper rectangle's top boundary.
		*/
		new_rect = new_rectangle(temp_rectangle.top, temp_rectangle.left, upper_rectangle->top - 1, 
			temp_rectangle.right);

		//sanity check
		if(new_rect == NULL) {
			//empty the whole list
			while(clipped_rectangle->count) {
				new_rect = delete_node(clipped_rectangle, 0);
				kfree(new_rect);
			}
			kfree(clipped_rectangle);
			return (List*)0;
		}

		//if successfully cut then add it to the list of clipped rectangles
		add_to_list(clipped_rectangle, new_rect);
		temp_rectangle.top = upper_rectangle->top;
	}

	//split by right edge
	if((upper_rectangle->right <= temp_rectangle.right) && (upper_rectangle->right >= temp_rectangle.left)) {
		/*create a new rectangle from 
		temp_rectangle bottom, top, right
		and upper rectangle's right boundary.
		*/
		new_rect = new_rectangle(temp_rectangle.top, upper_rectangle->right + 1, temp_rectangle.bottom, 
			temp_rectangle.right);

		//sanity check
		if(new_rect == NULL) {
			//empty the whole list
			while(clipped_rectangle->count) {
				new_rect = delete_node(clipped_rectangle, 0);
				kfree(new_rect);
			}
			kfree(clipped_rectangle);
			return (List*)0;
		}

		//if successfully cut then add it to the list of clipped rectangles
		add_to_list(clipped_rectangle, new_rect);
		temp_rectangle.right = upper_rectangle->right;
	}

	//split by bottom edge
	if((upper_rectangle->bottom <= temp_rectangle.bottom) && (upper_rectangle->bottom >= temp_rectangle.top)) {
		/*create a new rectangle from 
		temp_rectangle bottom, left, right
		and upper rectangle's bottom boundary.
		*/
		new_rect = new_rectangle(upper_rectangle->bottom + 1, temp_rectangle.left, temp_rectangle.bottom, 
			temp_rectangle.right);

		//sanity check
		if(new_rect == NULL) {
			//empty the whole list
			while(clipped_rectangle->count) {
				new_rect = delete_node(clipped_rectangle, 0);
				kfree(new_rect);
			}
			kfree(clipped_rectangle);
			return (List*)0;
		}

		//if successfully cut then add it to the list of clipped rectangles
		add_to_list(clipped_rectangle, new_rect);
		temp_rectangle.bottom = upper_rectangle->bottom;
	}

	return clipped_rectangle;
}

void horiz_line(context* cont, uint32_t x, uint32_t y, uint32_t length, uint32_t color) {
	draw_rectangle(curr_desktop->context, x, y, length, 1, color);
}

void vert_line(context* cont, uint32_t x, uint32_t y, uint32_t length, uint32_t color) {
	draw_rectangle(curr_desktop->context, x, y, 1, length, color);
}

/*
 * context_draw_rectangle
 *   DESCRIPTION: Draw a context plane
 *   INPUTS: context for framebuffer and plane coordinates
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */
void context_draw_rectangle(context* cont, uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color) {
	horiz_line(cont, x, y, width, color);
	vert_line(cont, x, y + 1, height - 2, color);
	horiz_line(cont, x, y + height - 1, width, color);
	vert_line(cont, x + width - 1, y + 1, height - 2, color);
}

/*
 * draw_clipped_rectangle
 *   DESCRIPTION: Draw a clipped rectangle
 *   INPUTS: context for framebuffer and rectangle coordinates
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */ 
void draw_clipped_rectangle(context* cont, uint32_t x, uint32_t y, uint32_t width, uint32_t height, 
	rectangle* clipped_rectangle, uint32_t color) {
	uint32_t current_x;
	uint32_t max_x = x + width;
	uint32_t max_y = y + width;

	//check bounds
	if(x < clipped_rectangle->left)
		x = clipped_rectangle->left;
	if(y < clipped_rectangle->top)
		y = clipped_rectangle->top;

	if(max_x > clipped_rectangle->right + 1)
		max_x = clipped_rectangle->right + 1;
	if(max_y > clipped_rectangle->bottom + 1)
		max_y = clipped_rectangle->bottom + 1;

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
 * draw_rectangle
 *   DESCRIPTION: Draw a rectangle
 *   INPUTS: context for framebuffer and rectangle coordinates
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */ 
void draw_rectangle(context* cont, uint32_t x, uint32_t y, uint32_t width, uint32_t height, uint32_t color) {
	// uint32_t current_x;
	// uint32_t max_x = x + width;
	// uint32_t max_y = y + height;

	// //check bounds
	// if(max_x > cont->width)
	// 	max_x = cont->width;
	// if(max_y > cont->height)
	// 	max_y = cont->height;

	// int index, plane, offset;
	// for(; y < max_y; y++) {
	// 	for(current_x = x; current_x < max_x; current_x++) {
	// 		index = y * cont->width + current_x;
	// 		plane = current_x % MODEX_PLANES;
	// 		offset = (current_x / MODEX_PLANES) + (y * cont->width / MODEX_PLANES);
	// 		cont->buffer[ ((MODE_X_MEM_SIZE / MODEX_PLANES) * plane) + offset] = color;
	// 	}
	// }

	if(cont->clipped_rectangles->count) {
		int i;
		rectangle* clipped_area;
		for(i = 0; i < cont->clipped_rectangles->count; i++) {
			clipped_area = (rectangle*)find_node(cont->clipped_rectangles, i);
			draw_clipped_rectangle(cont, x, y, width, height, clipped_area, color);
		}
	} else {
		rectangle split_area;
		split_area.top = 0;
		split_area.left = 0;
		split_area.bottom = cont->height - 1;
		split_area.right = cont->width - 1;
		draw_clipped_rectangle(cont, x, y, width, height, &split_area, color);
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

	//border
	context_draw_rectangle(curr_window->context, curr_window->x, curr_window->y, 
		curr_window->width, curr_window->height, WINDOW_BORDER);
	context_draw_rectangle(curr_window->context, curr_window->x + 1, curr_window->y + 1, 
		curr_window->width - 2, curr_window->height - 2, WINDOW_BORDER);
	context_draw_rectangle(curr_window->context, curr_window->x + 2, curr_window->y + 2, 
		curr_window->width - 4, curr_window->height - 4, WINDOW_BORDER);

	//title border
	horiz_line(curr_window->context, curr_window->x + 3, curr_window->x + 28,
		curr_window->width - 6, WINDOW_BORDER);
	horiz_line(curr_window->context, curr_window->x + 3, curr_window->x + 29,
		curr_window->width - 6, WINDOW_BORDER);
	horiz_line(curr_window->context, curr_window->x + 3, curr_window->x + 30,
		curr_window->width - 6, WINDOW_BORDER);

	//title
	draw_rectangle(curr_window->context, curr_window->x + 3, curr_window->y + 3,
		curr_window->width - 6, 8, WINDOW_TITLE);
	//window
	draw_rectangle(curr_window->context, curr_window->x + 3, curr_window->y + 14,
		curr_window->width - 6, curr_window->height - 17, WINDOW_BACKGROUND);
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
 * overlapping_windows
 *   DESCRIPTION: Find the overlapping windows of the current window
 *   INPUTS: desktop and window
 *   OUTPUTS: none
 *   RETURN VALUE: list of windows 
 */
List* overlapping_windows(desktop* curr_desktop, window* find_window) {
	int i;
	window* curr_window;

	List* overlap_windows = new_list();
	if(overlap_windows == NULL)
		return (List*)0;

	//get the window in the desktop list
	for(i = 0; i < curr_desktop->children->count; i++) {
		curr_window = (window*)find_node(curr_desktop->children, i);
		if(curr_window == find_window)
			break;
	}

	//add the remaning windows to the overlapping list
	for(; i < curr_desktop->children->count; i++) {
		curr_window = (window*)find_node(curr_desktop->children, i);
		/* check if the window overlaps and add it
		stack overflow:
		Cond1. If A's left edge is to the right of the B's right edge, - then A is Totally to right Of B
		Cond2. If A's right edge is to the left of the B's left edge, - then A is Totally to left Of B
		Cond3. If A's top edge is below B's bottom edge, - then A is Totally below B
		Cond4. If A's bottom edge is above B's top edge, - then A is Totally above B
		*/
		if(curr_window->x <= (find_window->x + find_window->width - 1) && (curr_window->x + curr_window->width - 1) >= find_window->x
			&& curr_window->y <= (find_window->y + find_window->height - 1) && (find_window->y + find_window->height - 1) >= find_window->y)
			add_to_list(overlap_windows, curr_window);
	}
	return overlap_windows;
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
				//check if this window was clicked (14 is the height of the title bar)
				if(curr_desktop->mouse_x >= curr_window->x && curr_desktop->mouse_x < (curr_window->x + curr_window->width) &&
					curr_desktop->mouse_y >= curr_window->y && curr_desktop->mouse_y < (curr_window->y + 14)) {
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
 * old_desktop_paint
 *   DESCRIPTION: Draw all the windows in the desktop list
 *   INPUTS: desktop
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */
void old_desktop_paint(desktop* curr_desktop) {
	// clear_desktop(curr_desktop);

	// int i;
	// window* curr_window;
	// for(i = 0; (curr_window = (window*)find_node(curr_desktop->children, i));i++)
	// 	window_paint(curr_window);

	// //draw a rectangle for a mouse
	// draw_rectangle(curr_desktop->context, curr_desktop->mouse_x, curr_desktop->mouse_y, 
	// 	2, 2, 40);
	// outputBuffer();

	uint32_t i;
	window* curr_window;
	rectangle* curr_rectangle;

	//clear the background
	clear_desktop(curr_desktop);
	clear_rectangles(curr_desktop->context);

	for(i = 0; (curr_window = (window*)find_node(curr_desktop->children, i));i++){
		curr_rectangle = new_rectangle(curr_window->y, curr_window->x, 
			curr_window->y + curr_window->height - 1, curr_window->x + curr_window->width - 1);
		add_rectangle(curr_desktop->context, curr_rectangle);
	}

	//draw the planes
	for(i = 0; i < curr_desktop->context->clipped_rectangles->count; i++) {
		curr_rectangle = (rectangle*)find_node(curr_desktop->context->clipped_rectangles, i);
		context_draw_rectangle(curr_desktop->context, curr_rectangle->left, curr_rectangle->top,
			curr_rectangle->right - curr_rectangle->left + 1, curr_rectangle->bottom - curr_rectangle->top + 1, 40);
	}

	//draw a rectangle for a mouse
	draw_rectangle(curr_desktop->context, curr_desktop->mouse_x, curr_desktop->mouse_y, 
		2, 2, 40);
	outputBuffer();
}

/*
 * desktop_paint
 *   DESCRIPTION: Draw all the windows in the desktop list
 *   INPUTS: desktop
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */
void desktop_paint(desktop* curr_desktop) {
	//draw a rectangle for the desktop
	rectangle* curr_rectangle = new_rectangle(0, 0, curr_desktop->context->height - 1, curr_desktop->context->width - 1);
	add_rectangle(curr_desktop->context, curr_rectangle);

	int i;
	window* curr_window;
	for(i = 0; i < curr_desktop->children->count; i++) {
		curr_window = (window*)find_node(curr_desktop->children, i);
		//subtract each window from desktop
		curr_rectangle = new_rectangle(curr_window->y, curr_window->x, curr_window->y + curr_window->height - 1,
			curr_window->x + curr_window->width - 1);
		remove_rectangle(curr_desktop->context, curr_rectangle);
		kfree(curr_rectangle);
	}

	draw_rectangle(curr_desktop->context, 0, 0, curr_desktop->context->width, curr_desktop->context->height, BACKGROUND_COLOR);
	clear_rectangles(curr_desktop->context);

	List* clipping_windows;
	window* clipping_window;
	//paint windows
	for(i = 0; i < curr_desktop->children->count; i++) {
		curr_window = (window*)find_node(curr_desktop->children, i);

		curr_rectangle = new_rectangle(curr_window->y, curr_window->x, curr_window->y + curr_window->height - 1,
			curr_window->x + curr_window->width - 1);
		add_rectangle(curr_desktop->context, curr_rectangle);

		clipping_windows = overlapping_windows(curr_desktop, curr_window);
		while(clipping_windows->count) {
			clipping_window = (window*)delete_node(clipping_windows, 0);
			if(clipping_window == curr_window)
				continue;

			curr_rectangle = new_rectangle(clipping_window->y, clipping_window->x, clipping_window->y + clipping_window->height - 1,
			clipping_window->x + clipping_window->width - 1);

			remove_rectangle(curr_desktop->context, curr_rectangle);
			kfree(curr_rectangle);
		}
		window_paint(curr_window);
		kfree(clipping_windows);
		clear_rectangles(curr_desktop->context);
	}

	//draw a rectangle for a mouse
	draw_rectangle(curr_desktop->context, curr_desktop->mouse_x, curr_desktop->mouse_y, 
		2, 2, 40);
	outputBuffer();
}

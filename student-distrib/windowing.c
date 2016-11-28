#include "windowing.h"

/*
 * new_window
 *   DESCRIPTION: Put VGA into text mode 3 (color text).
 *   INPUTS: clear_scr -- if non-zero, clear screens; otherwise, do not
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: may clear screens; writes font data to video memory
 */ 
window* new_window(uint16_t x, uint16_t y, uint16_t width, uint16_t height) {

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

	return new_window;
}

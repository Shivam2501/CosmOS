#include "pit.h"

void pit_init() {

	//set the mode
	outb(MODE, MODE_COMMAND_REGISTER);
	// setting the reload value
	outb(PIT_RELOAD_VALUE & 0xFF, PIT_CHANNEL_0); //set the low 8 bits (bitmask: 0xFF)
	outb(PIT_RELOAD_VALUE >> 8, PIT_CHANNEL_0); //set the high 8 bits (shift by 8)

	/* Enable the IRQ Port for PIT */
	enable_irq(PIT_IRQ);
}

void pit_handler() {
	send_eoi(PIT_IRQ);

	cli();
	context_switch();
	sti();
}

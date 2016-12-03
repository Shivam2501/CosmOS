#include "pit.h"

void pit_init() {


	/* Enable the IRQ Port for PIT */
	enable_irq(PIT_IRQ);
}

void pit_handler() {
	send_eoi(PIT_IRQ);

	cli();
	if(terminals[1].current_process != NULL || terminals[2].current_process != NULL ){
		context_switch();
	}
	sti();
}

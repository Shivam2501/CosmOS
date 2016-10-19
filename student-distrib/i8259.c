/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts
 * are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7 */
uint8_t slave_mask; /* IRQs 8-15 */

/*
 * i8259_init
 *   DESCRIPTION: Initialize the 8259 PIC.
 *   INPUTS: void
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */ 
void
i8259_init(void)
{
	/* cache the mask values */
	master_mask = inb(MASTER_8259_PORT);
	slave_mask = inb(SLAVE_8259_PORT);

	/* mask all the interrupts on the PIC */
	outb(IRQ_MASK, PIC_MASTER_DATA); /* mask all master interrupts */
	outb(IRQ_MASK, PIC_SLAVE_DATA); /* mask all slave interrupts */

	/* initialize the master PIC */
	outb(ICW1, MASTER_8259_PORT);	/* ICW1: select master PIC */
	outb(ICW2_MASTER, PIC_MASTER_DATA); /* ICW2: IRQ0-7 mapped to 0x20-0x27 */
	outb(ICW3_MASTER, PIC_MASTER_DATA);	/* Master has a slave on IR2 */
	outb(ICW4, PIC_MASTER_DATA); 

	/* initialize the slave PIC */
	outb(ICW1, SLAVE_8259_PORT);	/* ICW1: select slave PIC */
	outb(ICW2_SLAVE, PIC_SLAVE_DATA); /* ICW2: IRQ0-7 mapped to 0x28-0x2f */
	outb(ICW3_SLAVE, PIC_SLAVE_DATA);	/* Master has a slave on IR2 */
	outb(ICW4, PIC_SLAVE_DATA); 

	/* restore the mask values */
	outb(master_mask, PIC_MASTER_DATA);
	outb(slave_mask, PIC_SLAVE_DATA);
}

/*
 * enable_irq
 *   DESCRIPTION: Enable (unmask) the specified IRQ
 *   INPUTS: void
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */ 
void
enable_irq(uint32_t irq_num)
{
	uint8_t mask = 0xFE;
	//check if the irq is on slave or master
	if(irq_num>=NUMBER_SLAVE) {
		//if slave, compute the irq numer
		mask = mask << (irq_num-NUMBER_SLAVE);
		slave_mask = slave_mask & mask;
		outb(slave_mask, PIC_SLAVE_DATA);
	}
	else {
		mask = mask << irq_num;
		master_mask = master_mask & mask;
		outb(master_mask, PIC_MASTER_DATA);
	}
}

/*
 * disable_irq
 *   DESCRIPTION: Disable (mask) the specified IRQ
 *   INPUTS: void
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */
void
disable_irq(uint32_t irq_num)
{
	uint8_t mask = 0x01;
	//check if the irq is on slave or master
	if(irq_num>=NUMBER_SLAVE) {
		//if slave, compute the irq numer
		mask = mask << (irq_num-NUMBER_SLAVE);
		slave_mask = slave_mask | mask;
		outb(slave_mask, PIC_SLAVE_DATA);
	}
	else {
		mask = mask << irq_num;
		master_mask = master_mask | mask;
		outb(master_mask, PIC_MASTER_DATA);
	}
}

/*
 * send_eoi
 *   DESCRIPTION: Send end-of-interrupt signal for the specified IRQ
 *   INPUTS: void
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */
void
send_eoi(uint32_t irq_num)
{
	//check if the irq is on slave or master
	if(irq_num>=NUMBER_SLAVE) {
		//if slave, compute the irq numer
		outb(EOI | (irq_num-NUMBER_SLAVE), SLAVE_8259_PORT);
		outb(EOI | 2, MASTER_8259_PORT);
	}
	else {
		outb(EOI | irq_num, MASTER_8259_PORT);
	}
}

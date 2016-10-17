/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

#define PIC_MASTER				0x20		/* IO base address for master PIC */
#define PIC_SLAVE				0xA0		/* IO base address for slave PIC */
#define PIC_MASTER_COMMAND		PIC_MASTER
#define PIC_MASTER_DATA			(PIC_MASTER + 1)
#define PIC_SLAVE_COMMAND		PIC_SLAVE
#define PIC_SLAVE_DATA			(PIC_SLAVE + 1)

#define EOI_SIGNAL				0x20

/* Interrupt masks to determine which interrupts
 * are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7 */
uint8_t slave_mask; /* IRQs 8-15 */

/* Initialize the 8259 PIC */
void
i8259_init(void)
{
	/* cache the mask values */
	master_mask = inb(PIC_MASTER_DATA);
	slave_mask = inb(PIC_SLAVE_DATA);

	/* mask all the interrupts on the PIC */
	outb(0xFF, PIC_MASTER_DATA); /* mask all master interrupts */
	outb(0xFF, PIC_SLAVE_DATA); /* mask all slave interrupts */

	/* initialize the master PIC */
	outb_p(0x11, PIC_MASTER_COMMAND);	/* ICW1: select master PIC */
	outb_p(0x20 + 0, PIC_MASTER_DATA); /* ICW2: IRQ0-7 mapped to 0x20-0x27 */
	outb_p(0x04, PIC_MASTER_DATA);	/* Master has a slave on IR2 */

	/* initialize the slave PIC */
	outb_p(0x11, PIC_SLAVE_COMMAND);	/* ICW1: select slave PIC */
	outb_p(0x20 + 8, PIC_SLAVE_DATA); /* ICW2: IRQ0-7 mapped to 0x28-0x2f */
	outb_p(0x02, PIC_SLAVE_DATA);	/* Master has a slave on IR2 */

	/* restore the mask values */
	outb(master_mask, PIC_MASTER_DATA);
	outb(slave_mask, PIC_SLAVE_DATA);
}

/* Enable (unmask) the specified IRQ */
void
enable_irq(uint32_t irq_num)
{
	uint8_t mask = 0xFE;
	if(irq_num>=8) {
		mask = mask << (irq_num-8);
		slave_mask = slave_mask & mask;
		outb(slave_mask, PIC_MASTER_DATA);
	}
	else {
		mask = mask << irq_num;
		master_mask = master_mask & mask;
		outb(master_mask, PIC_MASTER_DATA);
	}
}

/* Disable (mask) the specified IRQ */
void
disable_irq(uint32_t irq_num)
{
	uint8_t mask = 0x01;
	if(irq_num>=8) {
		mask = mask << (irq_num-8);
		slave_mask = slave_mask | mask;
		outb(slave_mask, PIC_MASTER_DATA);
	}
	else {
		mask = mask << irq_num;
		master_mask = master_mask | mask;
		outb(master_mask, PIC_MASTER_DATA);
	}
}

/* Send end-of-interrupt signal for the specified IRQ */
void
send_eoi(uint32_t irq_num)
{
	if(irq_num>=8) {
		outb(EOI | (irq_num-8), PIC_SLAVE_COMMAND);
		outb(EOI | 2, PIC_MASTER_COMMAND);
	}
	else {
		outb(EOI | irq_num, PIC_MASTER_COMMAND);
	}
}

#include "rtc.h"
#include "i8259.h"
#include "lib.h"

/*
 * rtc_init
 *   DESCRIPTION: Initialise the RTC
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */ 
void rtc_init() {

	/* Write to RTC RAM, set the time base to 010 = 32.76 kHz*/
	//outb(0x20, RTC_DATA);
	int rate = 15;
	/* Select Register B and disable NMI*/
	outb(REGISTER_B_NMI, RTC_REGISTER);

	/* Read the current value in Register B. 
	This will set the index to register D*/
	uint8_t curr = inb(RTC_DATA);

	/* Select Register B(0x0B) and Disbale NMI(0x80)*/
	outb(REGISTER_B_NMI, RTC_REGISTER);

	/* Turn on the periodic interrupt at bit 6(Mask 0x40)*/
	outb(curr | 0x40, RTC_DATA);

	
	outb(REGISTER_A_NMI, RTC_REGISTER);
	/* Enable NMI */
	curr = inb(RTC_DATA);
	outb(REGISTER_A_NMI, RTC_REGISTER);
	/* Mask 0xF0*/
	outb((curr & 0xF0) | rate, RTC_DATA);

	/* Enable the IRQ Port for RTC*/
	enable_irq(RTC_IRQ);
}

/*
 * rtc_handler
 *   DESCRIPTION: RTC Interrupt Handler
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */ 
void rtc_handler() {
	/* Mask all interrupts 
	cli(); */

	//test_interrupts();

	/* Select register C*/
	outb(REGISTER_C, RTC_REGISTER);

	/* Clear the content */
	inb(RTC_DATA);

	send_eoi(RTC_IRQ);
	/* Unmask all interrupts 
	sti(); */
}

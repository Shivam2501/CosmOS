#include "rtc.h"
#include "i8259.h"
#include "lib.h"
#include "general_operations.h"
#include "file.h"

volatile int interrupt_flag;

/*
 * rtc_init
 *   DESCRIPTION: Initialise the RTC
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */ 
void rtc_init() {

	/* Write to RTC RAM, set the time base to 010 = 32.76 kHz */
	//outb(0x20, RTC_DATA);
	int rate = 15;
	/* Select Register B and disable NMI */
	outb(REGISTER_B_NMI, RTC_REGISTER);

	/* Read the current value in Register B. 
	This will set the index to register D */
	uint8_t curr = inb(RTC_DATA);

	/* Select Register B(0x0B) and Disbale NMI(0x80) */
	outb(REGISTER_B_NMI, RTC_REGISTER);

	/* Turn on the periodic interrupt at bit 6(Mask 0x40) */
	outb(curr | 0x40, RTC_DATA);

	
	outb(REGISTER_A_NMI, RTC_REGISTER);
	/* Enable NMI */
	curr = inb(RTC_DATA);
	outb(REGISTER_A_NMI, RTC_REGISTER);
	/* Mask 0xF0 */
	outb((curr & 0xF0) | rate, RTC_DATA);

	//interrupt flag set to 0 on initialization
	interrupt_flag = 0;

	/* Enable the IRQ Port for RTC */
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
	interrupt_flag = 1;
	/* Select register C*/
	outb(REGISTER_C, RTC_REGISTER);

	/* Clear the content */
	inb(RTC_DATA);

	send_eoi(RTC_IRQ);
	/* Unmask all interrupts 
	sti(); */
}

/*
 * set_frequency
 *   DESCRIPTION: Set the frequency to the input
 *   INPUTS: frequency 
 *   OUTPUTS: none
 *   RETURN VALUE: size of frequency on success, -1 on failure
 */ 
int32_t set_frequency(int32_t freq) {

	//check if frequency is a power of 2 and not greater than 1024 Hz
	if(freq > MAX_FREQUENCY)
		return -1;

	int32_t x = freq;
	while(((x % 2) ==0) && x > 1)
		x /= 2;

	if(x != 1)
		return -1;

	//find the rate based on input frequency
	uint8_t rate;

	switch(freq) {
		//rate calculated using freq = 32768 >> (rate-1)
		//if freq is 2 then rate = 0xF
		case 2: rate = 0xF;
				break;
		//if freq is 4 then rate = 0xE
		case 4: rate = 0xE;
				break;
		//if freq is 8 then rate = 0xD
		case 8: rate = 0xD;
				break;
		//if freq is 16 then rate = 0xC
		case 16: rate = 0xC;
				break;
		//if freq is 32 then rate = 0xB
		case 32: rate = 0xB;
				break;
		//if freq is 64 then rate = 0xA
		case 64: rate = 0xA;
				break;
		//if freq is 128 then rate = 0x9
		case 128: rate = 0x9;
				break;
		//if freq is 256 then rate = 0x8
		case 256: rate = 0x8;
				break;
		//if freq is 512 then rate = 0x7
		case 512: rate = 0x7;
				break;
		//if freq is 1024 then rate = 0x6
		case 1024: rate = 0x6;
				break;
	}

	disable_irq(RTC_IRQ);

	// set index to register A, disable NMI
	outb(REGISTER_A_NMI, RTC_REGISTER);
	// get initial value of register A
	uint8_t curr = inb(RTC_DATA);	

	// reset index to A
	outb(REGISTER_A_NMI, RTC_REGISTER);		
	// Mask 0xF0 
	outb((curr & 0xF0) | rate, RTC_DATA);

	enable_irq(RTC_IRQ);

	return MAX_BYTES;
}

/*
 * Start of System Calls
 */ 

/*
 * rtc_open
 *   DESCRIPTION: Initializes the rtc
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success
 */ 
int32_t rtc_open(const uint8_t* filename) {
	/*int index = DEFAULT_FD; 
	while(parent_pointer->FD[index].flags ==1 && index <FD_SIZE){
		index++;
	}

	parent_pointer->FD[index].inode = NULL;
	parent_pointer->FD[index].file_position = 0;
	parent_pointer->FD[index].flags = 1;
	*/
	//set the rtc to DEFAULT frequency
	set_frequency(DEFAULT_FREQUENCY);

	//interrupt flag set to 0 on rtc open
	interrupt_flag = 0;

	return 0;
	
}
 
/*
 * rtc_read
 *   DESCRIPTION: wait for an interrupt
 *   INPUTS: int32_t fd, uint8_t* buf, int32_t nbytes
 *   OUTPUTS: none
 *   RETURN VALUE: 0
 */ 
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes) {

	//wait until interrupt occurs
	while(!interrupt_flag);

	interrupt_flag = 0;

	return 0;
}

/*
 * rtc_write
 *   DESCRIPTION: set the rate of periodic interrupts
 *   INPUTS: int32_t fd, uint8_t* buf, int32_t nbytes
 *   OUTPUTS: none
 *   RETURN VALUE: bytes written on success, -1 on failure
 */ 
int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes) {

	int success;

	//check if frequency is 4 bytes
	if(nbytes != MAX_BYTES) {
		success = -1;
	} else {
		//set the frequency to input frequency
		success = set_frequency(*((int32_t *)buf));
	}

	return success;

}

/*
 * rtc_close
 *   DESCRIPTION: Close the rtc
 *   INPUTS: takes a file descriptor
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success
 */
int32_t rtc_close(int32_t fd) {

	//set the rtc to DEFAULT frequency
	set_frequency(DEFAULT_FREQUENCY);

	/*
	if(fd < DEFAULT_FD || fd >= FD_SIZE)
		return -1;

	parent_pointer->FD[fd].flags = 0;
	*/
	return 0;
}


/*
 * End of System Calls
 */
 
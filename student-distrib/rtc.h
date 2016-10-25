/* rtc.h - Defines for rtc handler
 * vim:ts=4 noexpandtab
 */

#ifndef _RTC_H
#define _RTC_H

#include "types.h"

#define RTC_IRQ						0x08
#define RTC_REGISTER				0x70
#define RTC_DATA					0x71

#define REGISTER_B_NMI				0x8B
#define REGISTER_A_NMI				0x8A
#define REGISTER_C					0x0C

#define DEFAULT_FREQUENCY			0x2
#define MAX_BYTES					0x4
#define MAX_FREQUENCY				1024

#define FREQ_2						2
#define FREQ_4						4
#define FREQ_8						8
#define FREQ_16						16
#define FREQ_32						32
#define FREQ_64						64
#define FREQ_128					128
#define FREQ_256					256
#define FREQ_512					512
#define FREQ_1024					1024

#define RATE_2						0xF
#define RATE_4						0xE
#define RATE_8						0xD
#define RATE_16						0xC
#define RATE_32						0xB
#define RATE_64						0xA
#define RATE_128					0x9
#define RATE_256					0x8
#define RATE_512					0x7
#define RATE_1024					0x6

/* Initialise the RTC */
void rtc_init();

/* RTC Interrupt Handler */
extern void rtc_handler();

int32_t set_frequency(int32_t freq);

int32_t rtc_open(void);

int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes) ;

int32_t rtc_write(int32_t fd, const void* buf, int32_t nbytes);

int32_t rtc_close(void);

#endif /* _RTC_H */

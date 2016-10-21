/* rtc.h - Defines for rtc handler
 * vim:ts=4 noexpandtab
 */

#ifndef _RTC_H
#define _RTC_H

#include "types.h"

#define RTC_IRQ					0x08
#define RTC_REGISTER			0x70
#define RTC_DATA				0x71

#define REGISTER_B_NMI			0x8B
#define REGISTER_A_NMI			0x8A
#define REGISTER_C				0x0C

#define DEFAULT_FREQUENCY		0x2
#define MAX_BYTES				0x4
#define MAX_FREQUENCY			1024

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

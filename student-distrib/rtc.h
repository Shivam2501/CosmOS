/* rtc.h - Defines for rtc handler
 * vim:ts=4 noexpandtab
 */

#ifndef _RTC_H
#define _RTC_H

#define RTC_IRQ					0x08
#define RTC_REGISTER			0x70
#define RTC_DATA				0x71

void rtc_init();

void rtc_handler();

#endif /* _KEYBOARD_H */
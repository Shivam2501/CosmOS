#ifndef _TEST_H
#define _TEST_H

#include "terminal.h"
#include "types.h"
#include "lib.h"
#include "rtc.h"
#include "file.h"


#define BUFSIZE 1024
#define RTC_PACKET_SIZE	4

extern volatile int stop;

int shell();
int test_rtc();
int test_cat(uint8_t* buf, int32_t cnt);
void write(uint8_t* string);

#endif /* _TEST_H */

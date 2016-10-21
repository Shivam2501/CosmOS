#include "keyboard.h"
#include "types.h"
#include "lib.h"
#include "rtc.h"

#define BUFSIZE 1024

int test_keyboard()
{
    int32_t cnt;
    uint8_t buf[BUFSIZE];

    rtc_open();

    uint32_t rtc[1];
    *rtc = 4; 
    rtc_write(1, rtc, 4);

    while(1) {
        if(rtc_read(1, rtc, 4) == 0) {
            terminal_write(1, (uint8_t*)"L", 1);
        }    
    }

    /*terminal_write(1, (uint8_t*)"Hi, what's your name? ", 22);
    if (-1 == (cnt = terminal_read (0, buf, BUFSIZE-1))) {
        return 3;
    }
    if (cnt > 0 && '\n' == buf[cnt - 1])
        cnt--;
    buf[cnt] = '\0';
    terminal_write(1, (uint8_t*)"Hello, ", 7);
    terminal_write(1, buf, cnt+1);*/

    return 0;
}


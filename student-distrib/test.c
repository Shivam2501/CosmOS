#include "keyboard.h"
#include "types.h"
#include "lib.h"

#define BUFSIZE 1024

int test_keyboard()
{
    int32_t cnt;
    uint8_t buf[BUFSIZE];

    terminal_write(1, (uint8_t*)"Hi, what's your name? ", 22);
    if (-1 == (cnt = terminal_read (0, buf, BUFSIZE-1))) {
        return 3;
    }
    if (cnt > 0 && '\n' == buf[cnt - 1])
        cnt--;
    buf[cnt] = '\0';
    terminal_write(1, (uint8_t*)"Hello, ", 7);
    terminal_write(1, buf, cnt);

    return 0;
}


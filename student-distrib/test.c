#include "test.h"

volatile int stop;

int frequency = 0;

void write(uint8_t* string) {
    terminal_write(1, string, strlen((int8_t*)string));
}

int test_rtc() {
    frequency++;
    if(frequency > 10)
        frequency = 1;

    int i = frequency;
    uint32_t rtc = 1;

    while(i != 0) {
        rtc = rtc * 2;
        i--;
    }

    int return_write = rtc_write(1, &rtc, RTC_PACKET_SIZE);
    if (return_write == -1) {
        write((uint8_t*)"Invalid frequency");
        return -1;
    }
    stop = 0;
    while(1) {
        if(rtc_read(1, &rtc, RTC_PACKET_SIZE) == 0) {
           write((uint8_t*)"1");
        }   

        if(stop)
            break;
    }
    return 0;
}

int test_ls() {
    int32_t cnt;
    uint8_t buf[BUFSIZE];

    while(1) {
        if (0 == (cnt = dir_read(1, buf, BUFSIZE-1))) {
            break;
        }

        if (cnt > 0 && '\n' == buf[cnt - 1])
            cnt--;
        buf[cnt] = '\n';

        terminal_write(1, buf, cnt+1);
    }

    return 0;
}

int test_cat(uint8_t* buf, int32_t cnt) {
    buf = buf + strlen((int8_t*)"cat ");

    int size = fs_size(1, buf, BUFSIZE-1);
    uint8_t buffer[size];

    strcpy((int8_t*)buffer, (int8_t*)buf);

    if (-1 == (cnt = fs_read(1, buffer, size-1))) {
        write((uint8_t*)"Invalid file name\n");
        return 0;
    }

    if (cnt > 0 && '\n' == buffer[cnt - 1])
        cnt--;
    buffer[cnt] = '\n';

    terminal_write(1, buffer, cnt+1);
    
    return 0;
}

int shell()
{
    int32_t cnt;
    uint8_t buf[BUFSIZE];
    
    stop = 0;

    write((uint8_t*)"Starting 391 Shell\n");

    while(1) {
        write((uint8_t*)"cosmOS> ");

        if (-1 == (cnt = terminal_read (0, buf, BUFSIZE-1))) {
            write((uint8_t*)"read from keyboard failed\n");
            return 3;
        }
        if (cnt > 0 && '\n' == buf[cnt - 1])
            cnt--;
        buf[cnt] = '\0';

        uint8_t* string;

        string = (uint8_t*)"rtc";
        if(0 == strncmp ((int8_t*)buf, (int8_t*)string, strlen((int8_t*)string))) {
           test_rtc();
        }

        string = (uint8_t*)"ls";
        if(0 == strncmp ((int8_t*)buf, (int8_t*)string, strlen((int8_t*)string))) {
            test_ls();
        }   

        string = (uint8_t*)"cat";
        if(0 == strncmp ((int8_t*)buf, (int8_t*)string, strlen((int8_t*)string))) {
            test_cat(buf, cnt);
        }   

        newline();
    }

    return 0;
}

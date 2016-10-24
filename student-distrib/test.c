#include "test.h"

volatile int stop;

int frequency = 0;

/*
 * write
 *   DESCRIPTION: Terminal write driver
 *   INPUTS: none
 *   OUTPUTS: writes to the screen
 *   RETURN VALUE: none
 */
void write(uint8_t* string) {
    terminal_write(1, string, strlen((int8_t*)string));
}

/*
 * test_rtc
 *   DESCRIPTION: Change the RTC frequency and wait for interrupt to print 1
 *   INPUTS: none
 *   OUTPUTS: Print 1 on every interrupt
 *   RETURN VALUE: none
 */
int test_rtc() {
    //increment the frequency
    frequency++;
    if(frequency > 10)
        frequency = 1;

    int i = frequency;
    uint32_t rtc = 1;

    //rtc is a power of 2
    while(i != 0) {
        rtc = rtc * 2;
        i--;
    }

    //write the rtc frequency
    int return_write = rtc_write(1, &rtc, RTC_PACKET_SIZE);
    if (return_write == -1) {
        write((uint8_t*)"Invalid frequency");
        return -1;
    }
    stop = 0;

    //wait for rtc interrupt and print 1
    while(1) {
        if(rtc_read(1, &rtc, RTC_PACKET_SIZE) == 0) {
           write((uint8_t*)"1");
        }   

        //CTRL+C stops the printing
        if(stop)
            break;
    }
    return 0;
}

/*
 * test_ls
 *   DESCRIPTION: Print the directory
 *   INPUTS: none
 *   OUTPUTS: Prints all the filenames in the directory
 *   RETURN VALUE: none
 */
int test_ls() {
    int32_t cnt;
    uint8_t buf[BUFSIZE];

    //directory read called until 0 is returned
    while(1) {
        if (0 == (cnt = dir_read(1, buf, BUFSIZE-1))) {
            break;
        }

        if (cnt > 0 && '\n' == buf[cnt - 1])
            cnt--;
        buf[cnt] = '\n';

        //write the file name using the terminal driver
        terminal_write(1, buf, cnt+1);
    }

    return 0;
}

/*
 * test_cat
 *   DESCRIPTION: Print the file
 *   INPUTS: none
 *   OUTPUTS: Prints all the file contents
 *   RETURN VALUE: none
 */
int test_cat(uint8_t* buf, int32_t cnt) {
    //increment the buffer to get filename argument
    buf = buf + strlen((int8_t*)"cat ");

    //get the size of the file
    int size = fs_size(1, buf, BUFSIZE-1);
    uint8_t buffer[size];

    strcpy((int8_t*)buffer, (int8_t*)buf);

    //read the file
    if (-1 == (cnt = fs_read(1, buffer, size-1))) {
        write((uint8_t*)"Invalid file name\n");
        return 0;
    }

    if (cnt > 0 && '\n' == buffer[cnt - 1])
        cnt--;
    buffer[cnt] = '\n';

    //print the buffer with file contents and of file size
    terminal_write(1, buffer, cnt+1);
    
    return 0;
}

/*
 * shell
 *   DESCRIPTION: Test for rtc and file
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */
int shell()
{
    int32_t cnt;
    uint8_t buf[BUFSIZE];
    
    stop = 0;

    write((uint8_t*)"Starting 391 Shell\n");

    while(1) {
        write((uint8_t*)"cosmOS> ");

        //get the user input
        if (-1 == (cnt = terminal_read(0, buf, BUFSIZE-1))) {
            write((uint8_t*)"read from keyboard failed\n");
            return 3;
        }
        if (cnt > 0 && '\n' == buf[cnt - 1])
            cnt--;
        buf[cnt] = '\0';

        uint8_t* string;

        //check if rtc is the input
        string = (uint8_t*)"rtc";
        if(0 == strncmp ((int8_t*)buf, (int8_t*)string, strlen((int8_t*)string))) {
           test_rtc();
        }

        //check if ls is the input
        string = (uint8_t*)"ls";
        if(0 == strncmp ((int8_t*)buf, (int8_t*)string, strlen((int8_t*)string))) {
            test_ls();
        }   

        //check if cat is the input
        string = (uint8_t*)"cat";
        if(0 == strncmp ((int8_t*)buf, (int8_t*)string, strlen((int8_t*)string))) {
            test_cat(buf, cnt);
        }   

        newline();
    }

    return 0;
}

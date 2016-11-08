#define ASM     1

#include "idt_system_call.h"

#calls handler for PIT
.globl call_handler
call_handler:

jmp 	*jump_table(, %eax, 4)

sys_1:
pushal
call 	mp3_kernel_halt
popal

sys_2:
pushal
call 	mp3_kernel_execute
popal

sys_3:
pushal
call 	fs_read
popal

sys_4:
pushal
call 	fs_write
popal

sys_5:
pushal
call 	general_open
popal

sys_6:
pushal
call 	fs_close
popal

sys_7:
pushal
call 	mp3_kernel_getargs
popal

sys_8:
pushal
call 	mp3_kernel_vidmap
popal

sys_9:
pushal
call 	mp3_kernel_set_handler
popal

sys_10:
pushal
call 	 mp3_kernel_sigreturn
popal


jump_table:
.long sys_1, sys_2, sys_3, sys_4, sys_5, sys_6, sys_7, sys_8, sys_9, sys_10

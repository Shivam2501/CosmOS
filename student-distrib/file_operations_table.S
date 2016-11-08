#define ASM     1

#include "idt_system_call.h"

#calls handler for RTC
.globl rtc_ops:
rtc_ops:
movl	16(%esp), %eax			//moving cmd to eax
//movl	12(%esp), %edx
jmp 	*jump_rtc(, %eax, 4)

open_rtc:
jmp 	done

close_rtc:
call 	rtc_close
jmp 	done

read_rtc:
pushl 	12(%esp)
pushl  	8(%esp)
pushl  	4(%esp)
call 	rtc_read 
subl  	$12, %esp
jmp 	done

write_rtc:
pushl 	12(%esp)
pushl  	8(%esp)
pushl  	4(%esp)
call 	rtc_write 
subl  	$12, %esp

jump_rtc:
.long	open_rtc, close_rtc, read_rtc, write_rtc

#calls handler for file
.globl file_ops:
file_ops:
jmp 	*jump_file(, %eax, 4)

open_file:
jmp 	done

close_file:
pushl   4(%esp)
call 	fs_close
subl    $4, %esp
jmp 	done

read_file:
pushl 	12(%esp)
pushl  	8(%esp)
pushl  	4(%esp)
call 	fs_read 
subl  	$12, %esp
jmp 	done

write_file:
pushl 	12(%esp)
pushl  	8(%esp)
pushl  	4(%esp)
call 	fs_write 
subl  	$12, %esp 

jump_file:
.long  open_file, close_file, read_file, write_file

#calls handler for directory
.globl dir_ops:
dir_ops:
jmp 	*jump_dir(, %eax, 4)

open_dir:
jmp 	done

close_dir:
pushl   4(%esp)
call 	dir_close
subl    $4, %esp
jmp 	done

read_dir:
pushl 	12(%esp)
pushl  	8(%esp)
pushl  	4(%esp)
call 	dir_read 
subl  	$12, %esp
jmp 	done

write_dir:
pushl 	12(%esp)
pushl  	8(%esp)
pushl  	4(%esp)
call 	dir_write 
subl  	$12, %esp 


jump_dir:
.long  open_dir, close_dir, read_dir, write_dir


done:
ret

#define ASM     1

#calls handler for PIT
.globl call_handler
call_handler:
pushl 	%ebp
movl 	%esp, %ebp
pushl 	%ebx
pushl 	%edi
pushl 	%esi

jmp 	*jump_table(, %eax, 4)

sys_1:
pushal
call 	syscall_halt
popal
jmp 	done

sys_2:
pushal
call 	syscall_execute
popal
jmp 	done

sys_3:
pushal
call 	syscall_read
popal
jmp 	done

sys_4:
pushal
call 	syscall_write
popal
jmp 	done

sys_5:
pushal
call 	syscall_open
popal
jmp 	done

sys_6:
pushal
call 	syscall_close
popal
jmp 	done

sys_7:
pushal
call 	syscall_getargs
popal
jmp 	done

sys_8:
pushal
call 	syscall_vidmap
popal
jmp 	done

sys_9:
pushal
call 	syscall_set_handler
popal
jmp 	done


sys_10:
pushal
call 	 syscall_sigreturn
popal
jmp 	done


done:
popl %edi
popl %esi
popl %ebx
leave
ret


jump_table:
.long sys_1, sys_2, sys_3, sys_4, sys_5, sys_6, sys_7, sys_8, sys_9, sys_10

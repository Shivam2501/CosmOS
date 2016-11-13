#define ASM     1

#calls handler for PIT
.globl call_handler
call_handler:
pushl 	%ebx
pushl 	%edi
pushl 	%esi

cmpl    $1,  %eax
jb      sys_fail 
cmpl    $10, %eax
ja  	sys_fail

pushl  	%edx
pushl   %ecx 
pushl   %ebx
jmp 	*jump_table(, %eax, 4)

sys_1:
call 	syscall_halt
jmp 	done

sys_2:
call 	syscall_execute
jmp 	done

sys_3:
call 	syscall_read
jmp 	done

sys_4:
call 	syscall_write
jmp 	done

sys_5:
call 	syscall_open
jmp 	done

sys_6:
call 	syscall_close
jmp 	done

sys_7:
call 	syscall_getargs
jmp 	done

sys_8:
call 	syscall_vidmap
jmp 	done

sys_9:
call 	syscall_set_handler
jmp 	done


sys_10:
call 	 syscall_sigreturn
jmp 	done

sys_fail:
call 	 syscall_fail
jmp 	done

done:
popl   	%ebx
popl   	%ecx 
popl   	%edx
popl 	%edi
popl 	%esi
popl 	%ebx
iret

jump_table:
.long  0, sys_1, sys_2, sys_3, sys_4, sys_5, sys_6, sys_7, sys_8, sys_9, sys_10

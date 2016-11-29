#define ASM     1

#calls handler for PIT
.globl call_handler
call_handler:
#push registers except eax which stores the return value
pushl 	%es 
pushl 	%ds
pushl  	%ebp
pushl 	%edx
pushl 	%ecx
pushl 	%ebx
pushl 	%edi
pushl 	%esi
pushfl

#push arguments for system call
pushl  	%edx
pushl   %ecx 
pushl   %ebx

#check if eax value (system call number) is within bounds 
cmpl    $1,  %eax
jl      sys_fail 
cmpl    $10, %eax
jg  	sys_fail

jmp 	*jump_table(, %eax, 4)

#call corresponding system calls using jump table
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

#call fail system call when system call number is not valid
sys_fail:
call 	syscall_fail
jmp 	done

done:
#pop corresponding arguments and callee-saved registers
popl   	%ebx
popl   	%ecx 
popl   	%edx

popfl
popl 	%esi
popl 	%edi
popl 	%ebx
popl 	%ecx
popl 	%edx
popl 	%ebp
popl 	%ds
popl 	%es
iret

jump_table:
.long  0, sys_1, sys_2, sys_3, sys_4, sys_5, sys_6, sys_7, sys_8, sys_9, sys_10

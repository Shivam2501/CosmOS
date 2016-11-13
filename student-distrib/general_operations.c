#include "general_operations.h"

PCB_t *parent_pointer = NULL; 
int32_t pid_tracker[MAX_NUM_PROCESS];					//index into pid_tracker is pid-1

int32_t init_FD(){
	//am setting stdin and stdout fd blocks
	file_array_t stdin;
	stdin.flags = 1;
	//jump_table directly jumpts to function
	ops_table_t op_table;
	op_table.open = &terminal_open;
	op_table.close = &terminal_close;
	op_table.read = &terminal_read;
	op_table.write = NULL;

	stdin.ops_table_ptr = op_table;
	stdin.file_position = 0; 
	stdin.inode = NULL;

	op_table.read = NULL;
	op_table.write = &terminal_write;

	file_array_t stdout;
	stdout.flags = 1; 
	stdout.ops_table_ptr = op_table;
	stdout.file_position = 0; 
	stdout.inode = NULL;

	parent_pointer->FD[0] = stdin;
	parent_pointer->FD[1] = stdout;

	return 0;
}

/*
 * general_open
 *   DESCRIPTION: Open the file
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success, -1 on failure
 */ 
int32_t syscall_open(const uint8_t* filename) {
	int index = DEFAULT_FD;

	while(parent_pointer->FD[index].flags == 1 && index < FD_SIZE){
		index++;
	}

	if(index == FD_SIZE)
		return -1;

	dentry_t dentry_file_info;
	if(read_dentry_by_name((uint8_t*)filename, &dentry_file_info) == 0) {	//if file is valid 
		switch(dentry_file_info.file_type){
			case 0:
				{
					ops_table_t op_table;
					op_table.open = &rtc_open;
					op_table.close = &rtc_close;
					op_table.read = &rtc_read;
					op_table.write = &rtc_write;
		
					parent_pointer->FD[index].ops_table_ptr = op_table;
					parent_pointer->FD[index].inode = NULL;
					parent_pointer->FD[index].file_position = 0;
					parent_pointer->FD[index].flags = 1;

					op_table.open(filename);
					break;
				}
			case 1:
				{
					ops_table_t op_table;										//creates jumptable for dir
					op_table.open = &dir_open;
					op_table.close = &dir_close;
					op_table.read = &dir_read;
					op_table.write = &dir_write;
			
					parent_pointer->FD[index].ops_table_ptr = op_table;			//sets ptr to jumptable in the struct for fd entry
					parent_pointer->FD[index].inode = NULL;
					parent_pointer->FD[index].file_position = 0;
					parent_pointer->FD[index].flags = 1;

					op_table.open(filename);
					break;
				}
			case 2:
				{
					ops_table_t op_table;
					op_table.open = &fs_open;
					op_table.close = &fs_close;
					op_table.read = &fs_read;
					op_table.write = &fs_write;

					parent_pointer->FD[index].ops_table_ptr = op_table;
					parent_pointer->FD[index].inode = &(dentry_file_info.inode);
					parent_pointer->FD[index].file_position = 0;
					parent_pointer->FD[index].flags = 1;

					op_table.open(filename);
					break;
				}
		}
	} else {
		return -1;
	}

	return index; 
}


int32_t syscall_read(int32_t fd, void* buf, int32_t nbytes) {
	if(fd < DEFAULT_FD || fd >= FD_SIZE)
		return -1;

	return parent_pointer->FD[fd].ops_table_ptr.read(fd, buf, nbytes);
}

int32_t syscall_write(int32_t fd, const void* buf, int32_t nbytes) {
	if(fd < DEFAULT_FD || fd >= FD_SIZE)
		return -1;
	
	return parent_pointer->FD[fd].ops_table_ptr.write(fd, buf, nbytes);
}

int32_t syscall_close(int32_t fd) {
	if(fd < DEFAULT_FD || fd >= FD_SIZE)
		return -1;

	parent_pointer->FD[fd].flags = 0;
	return parent_pointer->FD[fd].ops_table_ptr.close(fd);
}


int32_t syscall_getargs (uint8_t* buf, int32_t nbytes)
{
	return 0;

}
int32_t syscall_vidmap (uint8_t** screen_start)
{
	return 0;

}
int32_t syscall_set_handler (int32_t signum, void* handler_address)
{
	return 0;
}
int32_t syscall_sigreturn (void)
{
	return 0;

}

int32_t syscall_halt (uint8_t status){

	int i = 0;
	uint32_t parent_pid = parent_pointer->parent_ptr;
	uint32_t current_pid = parent_pointer->pid;

	//if trying to halt shell
	if(parent_pid == current_pid) {
		printf("Cannot close shell\n");
		return -1;
	}

	//get parent pcb
	PCB_t* parent_process = (PCB_t*)(KERNEL_PROCESS_START - (parent_pid+1)*KERNEL_STACK_SIZE);
	PCB_t* current_process = (PCB_t*)(KERNEL_PROCESS_START - (current_pid+1)*KERNEL_STACK_SIZE);

	parent_pointer = parent_process;
	pid_tracker[current_process->pid] = 0; 

	for(i = 0; i < MAX_OPEN_FILES; i++) {
		if(current_process->FD[i].flags)
			syscall_close(i);
	}

	//paging
	add_paging(PAGE_DIR_ENTRY, (KERNEL_PROCESS_START + (parent_process->pid)*KERNEL_PROCESS_SIZE));

	tss.ss0 = KERNEL_DS;
	tss.esp0 = current_process->tss.esp;

	uint32_t new_status = status;

	asm volatile("                  	\n\
			movl 	%0, %%esp			\n\
			movl	%1, %%ebp 			\n\
			movl	%2, %%eax 			\n\
			jmp 	LEAVE_RET 			\n\
			"
			: 
			: "r"(current_process->tss.esp), "r"(current_process->tss.ebp), "r"(new_status)
			: "memory", "cc", "%eax"	
			);

	return 0;
}

int32_t syscall_execute (const uint8_t* command){
	int i =0; 																		//set stdin, stdout
	uint8_t argument[MAX_BUFFER_SIZE]; 

	while(command[i]!= ' ')															//get first word
	{
		argument[i] = command[i];
		i++;
	}
	argument[i] = '\0';

	uint8_t buf[EXE_BUF_SIZE];

	dentry_t dentry_file_info;
	if(read_dentry_by_name((uint8_t*)argument, &dentry_file_info) != 0)
		return -1;

	read_data(dentry_file_info.inode, 0, buf, EXE_BUF_SIZE);

	if(buf[0] != 0x7f || buf[1] != 0x45 || buf[2] != 0x4c || buf[3] != 0x46)		//if not executable
		return -1; 

	for(i = 0; i < MAX_NUM_PROCESS; i++){											//find empty process 
		if(pid_tracker[i] == 0)
			break;
	}

	if(i == MAX_NUM_PROCESS){
		printf("No processes free");
		return -1;
	}

	/*Paging*/
	add_paging(PAGE_DIR_ENTRY, (KERNEL_PROCESS_START + i*KERNEL_PROCESS_SIZE));

	/*Load file in memory*/
	read_data(dentry_file_info.inode, 0, (uint8_t*)VIRTUAL_ADDRESS_PROGRAM, 100000);

	pid_tracker[i] = 1; 															//current process is being used
	PCB_t* pcb = (PCB_t*)(KERNEL_PROCESS_START - (i+1)*KERNEL_STACK_SIZE); 
	pcb->pid = i;

	asm volatile("                  \n\
			movl    %%cr3, %0   	\n\
			movl 	%%esp, %1	    \n\
			movl    %%ebp, %2		\n\
			pushfl					\n\
			popl	%3				\n\
			"
			: "=S"(pcb->tss.cr3), "=b"(pcb->tss.esp), "=c"(pcb->tss.ebp), "=d"(pcb->tss.eflags)
			:
			: "memory", "cc"
			);

	tss.ss0 = KERNEL_DS;
	tss.esp0 = KERNEL_PROCESS_START - i*KERNEL_STACK_SIZE - 4;	

	if(parent_pointer == NULL) {
		pcb->parent_ptr = pcb->pid;
	} else {
		pcb->parent_ptr = parent_pointer->pid;	
	}

	//save current pcb for next process
	parent_pointer = pcb;
	init_FD();

	read_data(dentry_file_info.inode, EIP_READ_OFFSET, buf, EXE_BUF_SIZE);
	//move data segment, push ss, esp, eflags, cs, eip 
	uint32_t entrypoint = *((uint32_t*)buf);

	//printf("%#x\n", entrypoint);

	asm volatile("                  	\n\
			mov 	%0, %%ds			\n\
			pushl   %0					\n\
			pushl   %1					\n\
			pushfl						\n\
			popl    %%eax				\n\
			orl  	%2, %%eax 			\n\
			pushl   %%eax				\n\
			pushl   $0x23				\n\
			pushl   %3					\n\
			iret 						\n\
			LEAVE_RET:					\n\
			leave 						\n\
			ret 						\n\
			"
			: 
			: "r"(USER_DS), "r"(ESP_VALUE), "r"(EFLAGS_VALUE),"r"(entrypoint)
			: "memory", "cc", "%eax"	
			);

	return 0;
}

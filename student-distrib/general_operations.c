#include "general_operations.h"

PCB_t *parent_pointer = NULL; 
int32_t pid_tracker[MAX_NUM_PROCESS];					//index into pid_tracker is pid-1


/*
 * init_FD
 *   DESCRIPTION: Sets the stdin and stdout FD array entries for each process
 *   INPUTS: none
 *   OUTPUTS:
 *   RETURN VALUE: 0
 */ 
int32_t init_FD(){
	file_array_t stdin;
	stdin.flags = 1;																	//these are in use, sets values for stdin
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

	file_array_t stdout;																//sets values for stdout
	stdout.flags = 1; 
	stdout.ops_table_ptr = op_table;
	stdout.file_position = 0; 
	stdout.inode = NULL;

	parent_pointer->FD[0] = stdin;
	parent_pointer->FD[1] = stdout;

	return 0;
}

/*
 * syscall_open
 *   DESCRIPTION: Open the file
 *   INPUTS: filename
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success, -1 on failure
 */ 
int32_t syscall_open(const uint8_t* filename) {
	int index = DEFAULT_FD;
	//find free fd array
	while(parent_pointer->FD[index].flags == 1 && index < FD_SIZE){								
		index++;
	}

	if(index == FD_SIZE)
		return -1;

	//if file is valid
	dentry_t dentry_file_info;
	if(read_dentry_by_name((uint8_t*)filename, &dentry_file_info) == 0) {				 
		switch(dentry_file_info.file_type){
			//rtc handling
			case 0:	
				{
					//make jump table to specific rtc functions
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
				//directory open
			case 1:
				{
					//creates jumptable for dir
					ops_table_t op_table;												
					op_table.open = &dir_open;
					op_table.close = &dir_close;
					op_table.read = &dir_read;
					op_table.write = &dir_write;
			
					//sets ptr to jumptable in the struct for fd entry
					parent_pointer->FD[index].ops_table_ptr = op_table;					
					parent_pointer->FD[index].inode = NULL;
					parent_pointer->FD[index].file_position = 0;
					parent_pointer->FD[index].flags = 1;

					op_table.open(filename);
					break;
				}
				//file open
			case 2:
				{
					//make jumptable for file specific functions
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

	//returns the file descriptor
	return index; 
}

/*
 * syscall_read
 *   DESCRIPTION: read the file
 *   INPUTS: file descriptor, buffer to read to, the number of bytes to read
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success, -1 on failure
 */ 
int32_t syscall_read(int32_t fd, void* buf, int32_t nbytes) {

	//check if file descriptor is not valid
	if(fd < 0 || fd >= FD_SIZE)  
		return -1;

	//call read specific to type
	return parent_pointer->FD[fd].ops_table_ptr.read(fd, buf, nbytes);
}

/*
 * syscall_write
 *   DESCRIPTION: Write the file
 *   INPUTS: file descriptor, buffer to write to, number of bytes to write
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success, -1 on failure
 */ 
int32_t syscall_write(int32_t fd, const void* buf, int32_t nbytes) {
	//check if file descriptor is valid
	if(fd < 0 || fd >= FD_SIZE)
		return -1;

	//call write specific to type
	return parent_pointer->FD[fd].ops_table_ptr.write(fd, buf, nbytes);
}

/*
 * syscall_close
 *   DESCRIPTION: close the file
 *   INPUTS: filename
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success, -1 on failure
 */ 
int32_t syscall_close(int32_t fd) {
	//check if file descriptor is valid
	if(fd < 0 || fd >= FD_SIZE)
		return -1;

	//set flag to not being used now
	parent_pointer->FD[fd].flags = 0;
	//call close specific to type
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

/*
 * syscall_fail
 *   DESCRIPTION: checks if an invalid system call was called
 *   INPUTS: 
 *   OUTPUTS: prints invalid system call
 *   RETURN VALUE: 0 
 */ 
int32_t syscall_fail (void)
{
	printf("Invalid system call\n");
	return 0;

}

/*
 * syscall_halt
 *   DESCRIPTION: halt the process
 *   INPUTS: status
 *   OUTPUTS:
 *   RETURN VALUE: 0 on success
 */ 
int32_t syscall_halt (uint8_t status){

	int i = 0;
	//obtain current and parent's pids
	uint32_t parent_pid = parent_pointer->parent_ptr;
	uint32_t current_pid = parent_pointer->pid;

	//get parent pcb
	PCB_t* parent_process = (PCB_t*)(KERNEL_PROCESS_START - (parent_pid+1)*KERNEL_STACK_SIZE);
	PCB_t* current_process = (PCB_t*)(KERNEL_PROCESS_START - (current_pid+1)*KERNEL_STACK_SIZE);

	pid_tracker[current_process->pid] = 0; 

	//close each fd that is open in current process
	for(i = 0; i < MAX_OPEN_FILES; i++) {
		if(current_process->FD[i].flags)
			syscall_close(i);
	}

	//paging to parent process
	add_paging(PAGE_DIR_ENTRY, (KERNEL_PROCESS_START + (parent_process->pid)*KERNEL_PROCESS_SIZE));

	//tss.ss0 = KERNEL_DS;
	tss.esp0 = KERNEL_PROCESS_START - parent_pid*KERNEL_STACK_SIZE - PAGE_ALIGNMENT;

	//if trying to halt shell
	if(parent_pid == current_pid) {
		parent_pointer = NULL;
		syscall_execute((uint8_t*)"shell");
		return -1;
	}

	//parent becomes current process's parent
	parent_pointer = parent_process;
	
	//have to return status
	uint32_t new_status = status;

	//push correct values to jump
	asm volatile("                  	\n\
			movl 	%0, %%esp			\n\
			movl	%1, %%ebp 			\n\
			movl	%2, %%eax 			\n\
			jmp 	LEAVE_RET 			\n\
			"
			: 
			: "r"(current_process->tss.esp), "r"(current_process->tss.ebp), "r"(new_status)
			: "memory", "cc", "eax"	
			);

	return 0;
}

/*
 * syscall_execute
 *   DESCRIPTION: execute the process
 *   INPUTS: command
 *   OUTPUTS: none
 *   RETURN VALUE: 0
 */ 
int32_t syscall_execute (const uint8_t* command){
	int i = 0; 																		//set stdin, stdout
	uint8_t argument[MAX_BUFFER_SIZE]; 

	//parse args to get the first word
	while(command[i] != '\0')															
	{
		if(command[i] != ' '){
			argument[i] = command[i];
		}else{
			break;
		}

		i++;
	}
	//printf("bbb%sbbb\n",argument);
	argument[i] = '\0';
	//printf("ccc%sccc\n",argument);

	uint8_t buf[EXE_BUF_SIZE];

	dentry_t dentry_file_info;
	//get inode value, if invalid file then return -1
	if(read_dentry_by_name((uint8_t*)argument, &dentry_file_info) != 0)
		return -1;

	//obtain first four bytes to check if executable
	read_data(dentry_file_info.inode, 0, buf, EXE_BUF_SIZE);

	//check if not executable
	if(buf[0] != 0x7f || buf[1] != 0x45 || buf[2] != 0x4c || buf[3] != 0x46)		
		return -1; 

	//find empty process 
	for(i = 0; i < MAX_NUM_PROCESS; i++){											
		if(pid_tracker[i] == 0)
			break;
	}

	if(i == MAX_NUM_PROCESS){
		printf("No processes free");
		return -1;
	}

	/*Paging*/
	//change entry in page directory
	add_paging(PAGE_DIR_ENTRY, (KERNEL_PROCESS_START + i*KERNEL_PROCESS_SIZE));

	/*Load file in memory*/
	read_data(dentry_file_info.inode, 0, (uint8_t*)VIRTUAL_ADDRESS_PROGRAM, FILE_SIZE);

	//current process is being used -> mark as used
	pid_tracker[i] = 1;
	//create pcb at correct memory location 															
	PCB_t* pcb = (PCB_t*)(KERNEL_PROCESS_START - (i+1)*KERNEL_STACK_SIZE); 
	//save process number
	pcb->pid = i;

	//save cr3, esp, ebp registers in tss within pcb
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

	//set tss values
	tss.ss0 = KERNEL_DS;
	tss.esp0 = KERNEL_PROCESS_START - i*KERNEL_STACK_SIZE - PAGE_ALIGNMENT;

	//set the parent pointer
	if(parent_pointer == NULL) {
		pcb->parent_ptr = pcb->pid;
	} else {
		pcb->parent_ptr = parent_pointer->pid;	
	}

	//save current pcb for next process
	parent_pointer = pcb;
	init_FD();

	//read 24-27 bytes of executable file which serves as entrypoint
	read_data(dentry_file_info.inode, EIP_READ_OFFSET, buf, EXE_BUF_SIZE);
	//move data segment, push ss, esp, eflags, cs, eip 
	uint32_t entrypoint = *((uint32_t*)buf);

	//push correct values onto  stack for iret context
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
			: "memory", "cc", "eax"	
			);

	return 0;
}

PCB_t* get_current_pcb() {
	return parent_pointer;
}

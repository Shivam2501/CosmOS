#include "syscalls.h"

//PCB_t *terminals[active_terminal].current_process = NULL; 
int32_t pid_tracker[MAX_NUM_PROCESS];					//index into pid_tracker is pid-1

//ops_table for rtc, file and directory 
ops_table_t op_table_rtc = { &rtc_open, &rtc_close, &rtc_read, &rtc_write };
ops_table_t op_table_dir = { &dir_open, &dir_close, &dir_read, &dir_write };
ops_table_t op_table_file = { &fs_open, &fs_close, &fs_read, &fs_write };

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

	//terminals[active_terminal].buffer_index = 0;
	terminals[active_terminal].current_process->FD[0] = stdin;
	terminals[active_terminal].current_process->FD[1] = stdout;

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

	//if filename is NULL
	if(filename == NULL)
		return -1;

	int index = DEFAULT_FD;
	//find free fd array
	while(terminals[active_terminal].current_process->FD[index].flags == 1 && index < FD_SIZE){								
		index++;
	}

	if(index == FD_SIZE)
		return -1;

	//if file is valid
	dentry_t dentry_file_info;
	if(read_dentry_by_name((uint8_t*)filename, &dentry_file_info) == 0) {				 
		switch(dentry_file_info.file_type){
			//rtc handlingcreated
			case 0:	
				{
					terminals[active_terminal].current_process->FD[index].ops_table_ptr = op_table_rtc;
					terminals[active_terminal].current_process->FD[index].inode = NULL;
					terminals[active_terminal].current_process->FD[index].file_position = 0;
					terminals[active_terminal].current_process->FD[index].flags = 1;

					terminals[active_terminal].current_process->FD[index].ops_table_ptr.open(filename);
					break;
				}
				//directory open
			case 1:
				{
					//sets ptr to jumptable in the struct for fd entry
					terminals[active_terminal].current_process->FD[index].ops_table_ptr = op_table_dir;					
					terminals[active_terminal].current_process->FD[index].inode = NULL;
					terminals[active_terminal].current_process->FD[index].file_position = 0;
					terminals[active_terminal].current_process->FD[index].flags = 1;

					terminals[active_terminal].current_process->FD[index].ops_table_ptr.open(filename);
					break;
				}
				//file open
			case 2:
				{
					terminals[active_terminal].current_process->FD[index].ops_table_ptr = op_table_file;
					terminals[active_terminal].current_process->FD[index].inode = dentry_file_info.inode;
					terminals[active_terminal].current_process->FD[index].file_position = 0;
					terminals[active_terminal].current_process->FD[index].flags = 1;

					terminals[active_terminal].current_process->FD[index].ops_table_ptr.open(filename);
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
	if(fd < 0 || fd >= FD_SIZE || terminals[active_terminal].current_process->FD[fd].flags == 0)  
		return -1;

	//check if buf is NULL or the read function is NULL
	if(terminals[active_terminal].current_process->FD[fd].ops_table_ptr.read == NULL || buf == NULL)
		return -1;

	//call read specific to type
	return terminals[active_terminal].current_process->FD[fd].ops_table_ptr.read(fd, buf, nbytes);
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
	if(fd < 0 || fd >= FD_SIZE || terminals[active_terminal].current_process->FD[fd].flags == 0)
		return -1;

	//check if buf is NULL or the write function is NULL
	if(terminals[active_terminal].current_process->FD[fd].ops_table_ptr.write == NULL || buf == NULL)
		return -1;

	//call write specific to type
	return terminals[active_terminal].current_process->FD[fd].ops_table_ptr.write(fd, buf, nbytes);
}

/*
 * syscall_close
 *   DESCRIPTION: close the file
 *   INPUTS: filename
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success, -1 on failure
 */ 
int32_t syscall_close(int32_t fd) {
	//check if file descriptor is valid, can't close stdin or stdout(fd < 2)
	if(fd < 2 || fd >= FD_SIZE || terminals[active_terminal].current_process->FD[fd].flags == 0)
		return -1;

	//set flag to not being used now
	terminals[active_terminal].current_process->FD[fd].flags = 0;
	//call close specific to type
	return terminals[active_terminal].current_process->FD[fd].ops_table_ptr.close(fd);
}

/*
 * syscall_getargs
 *   DESCRIPTION: copy nbytes of args into user-level buffer
 *   INPUTS: a buffer to copy to, number of bytes to copy
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success, -1 if arguments + null don't fit in buffer
 */ 
int32_t syscall_getargs (uint8_t* buf, int32_t nbytes)
{
	if(strlen((int8_t*)terminals[active_terminal].current_process->arguments) > nbytes || buf == NULL)				//the arguments and a terminal NULL (0-byte) do not fit in the buffer
		return -1; 	

	memcpy(buf, terminals[active_terminal].current_process->arguments, nbytes);
	return 0;
}

/*
 * syscall_vidmap
 *   DESCRIPTION: assign video mem to user space
 *   INPUTS: address which should point to the virtual address where video mem is mapped
 *   OUTPUTS: none
 *   RETURN VALUE: address of video memory
 */ 
int32_t syscall_vidmap (uint8_t** screen_start)
{
	if(screen_start == NULL || screen_start < (uint8_t**)USER_PROGRAM_START || screen_start > (uint8_t**)ESP_VALUE)
		return -1;

	//add paging from 132MB -> xB80000
	add_paging_4kb(_132MB, VIDEO_MEM, 1);
	//assign the address pointing to the video mem
	*screen_start = (uint8_t*)_132MB;
	return _132MB;
}

int32_t syscall_set_handler (int32_t signum, void* handler_address)
{
	return -1;
}

int32_t syscall_sigreturn (void)
{
	return -1;

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
	//printf("Invalid system call\n");
	return -1;

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
	uint32_t parent_pid = terminals[active_terminal].current_process->parent_ptr;
	uint32_t current_pid = terminals[active_terminal].current_process->pid;

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
		terminals[active_terminal].current_process = NULL;
		syscall_execute((uint8_t*)"shell");
		return -1;
	}

	//clear thekeyboard buffer
	clear_buffer();

	//parent becomes current process's parent
	terminals[active_terminal].current_process = parent_process;
	
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
	int i = 0, j=0; 																		//set stdin, stdout
	
	//check if command is NULL
	if(command == NULL)
		return -1;

	uint8_t first_command[MAX_BUFFER_SIZE], arg_buf[MAX_BUFFER_SIZE]; 

	memset(first_command, '\0', MAX_BUFFER_SIZE);
	memset(arg_buf, '\0', MAX_BUFFER_SIZE);

	//ignore spaces at the start before the command
	while(command[i] == ' ')															
		i++;

	//parse args to get the first word
	while(command[i] != '\0')															
	{
		if(command[i] == ' ')
			break;
		first_command[j] = command[i]; 		//save command in one buffer
		i++;
		j++;
	}

	//ignore spaces at the start before the command
	while(command[i] == ' ')															
		i++;

	j = 0;
	//parse the argument and store
	while(command[i] != '\0') {	
		arg_buf[j] = command[i];
		i++;
		j++;
	}
	
	first_command[i] = '\0';
	arg_buf[j] = '\0';

	uint8_t buf[EXE_BUF_SIZE];

	dentry_t dentry_file_info;
	//get inode value, if invalid file then return -1
	if(read_dentry_by_name((uint8_t*)first_command, &dentry_file_info) != 0)
		return -1;

	//obtain first four bytes to check if executable
	read_data(dentry_file_info.inode, 0, buf, EXE_BUF_SIZE);

	//check if not executable (0x7f: DEL, 0x45: E, 0x4c: L, 0x46:F)
	if(buf[0] != 0x7f || buf[1] != 0x45 || buf[2] != 0x4c || buf[3] != 0x46)		
		return -1; 

	//find empty process 
	if(-1 == (i = get_available_pid())){
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
	if(terminals[active_terminal].current_process == NULL) {
		pcb->parent_ptr = pcb->pid;
	} else {
		pcb->parent_ptr = terminals[active_terminal].current_process->pid;	
	}

	memset(pcb->arguments, '\0', MAX_BUFFER_SIZE);
	//copy arguments into pcb
	memcpy(pcb->arguments, arg_buf, strlen((int8_t*)arg_buf));

	//save current pcb for next process
	terminals[active_terminal].current_process = pcb;
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

/*
 * get_available_pid
 *   DESCRIPTION: return the pid which is free
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: pid on success, -1 on fail
 */
int get_available_pid() {
	int i;
	for(i = 0; i < MAX_NUM_PROCESS; i++){											
		if(pid_tracker[i] == 0)
			return i;
	}

	if(i == MAX_NUM_PROCESS){
		printf("No processes free\n");
		return -1;
	}
	return 0;
}

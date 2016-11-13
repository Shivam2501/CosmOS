#include "general_operations.h"

int32_t pid_tracker[max_num_processes];					//index into pid_tracker is pid-1

int32_t init_FD(){
	//am setting stdin and stdout fd blocks
	file_array_t stdin;
	stdin.flags = 1;
	//jump_table directly jumpts to function
	ops_table_t op_table;
	op_table.open = NULL;
	op_table.close = NULL;
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

	while(parent_pointer->FD[index].flags ==1 && index <FD_SIZE){
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
	return parent_pointer->FD[fd].ops_table_ptr.read(fd, buf, nbytes);
}

int32_t syscall_write(int32_t fd, const void* buf, int32_t nbytes) {
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
	return 0;
}
int32_t syscall_execute (const uint8_t* command){
	int i =0; 																		//set stdin, stdout
	uint8_t* argument; 

	while(command[i]!= ' ')															//get first word
	{
		argument[i] = command[i];
		i++;
	}
	uint8_t buf[EXE_BUF_SIZE];


	dentry_t dentry_file_info;
	if(read_dentry_by_name((uint8_t*)argument, &dentry_file_info) != 0);
		return -1;

	read_data(dentry_file_info.inode, 0, buf, EXE_BUF_SIZE);

	if(buf[0] != 0x7f || buf[1] != 0x45 || buf[2] != 0x4c || buf[3] != 0x46)		//if not executable
		return -1; 
	
	for(i = 0; i < max_num_processes; i++){											//find empty process 
		if(pid_tracker[i] == 0)
			break;
	}
	if(i == max_num_processes){
		printf("No processes free");
		return -1;
	}

	/*Paging*/
	page_directory[PAGE_DIR_ENTRY] = (KERNEL_PROCESS_START + i*KERNEL_PROCESS_SIZE) | PS | READ_WRITE | PRESENT;
	loadPageDirectory((uint32_t*) page_directory);


	/*Load file in memory*/
	uint32_t buffer = KERNEL_PROCESS_START;
	read_data(dentry_file_info.inode, 0, (uint8_t*)buffer, BUF_SIZE);

	pid_tracker[i] = 1; 															//current process is being used
	PCB_t* pcb = (PCB_t*)(KERNEL_PROCESS_START - (i+1)*KERNEL_STACK_SIZE); 
	pcb->pid = i;
	
 																				//save registers, cr3, flags in pcb
	asm volatile("                  \n\
			movl    %%cr3, %0      	\n\
			movl 	%%esp, %1		\n\
			movl    %%ebp, %2		\n\
			movl 	%%eflags, %3   	\n\
			"
			: "=a"(pcb->tss.cr3), "=b"(pcb->tss.esp), "=c"(pcb->tss.ebp), "=d"(pcb->tss.eflags)
			:
			: "eax", "ebx", "ecx", "edx"	
			);

	pcb->tss.esp0 = KERNEL_PROCESS_START - i*KERNEL_STACK_SIZE;	
	pcb->parent_ptr = parent_pointer;												//save current pcb for next process
	parent_pointer = pcb;

	init_FD();	

	read_data(dentry_file_info.inode, EIP_READ_OFFSET, buf, EXE_BUF_SIZE);

	//move data segment, push ss, esp, eflags, cs, eip 
	asm volatile("                  	\n\
			movl 	%0, %%ds			\n\
			pushl   %0				    \n\
			pushl   %1					\n\
			pushl   %2					\n\
			pushl   %3					\n\
			pushl   %4					\n\
			iret 						\n\
			LEAVE_RET:					\n\
			leave 						\n\
			ret 						\n\
			"
			: 
			: "a"(USER_DS), "b"(ESP_VALUE), "c"(EFLAGS_VALUE), "d"(USER_CS), "e"(buf)
			: "eax", "ebx", "ecx"	
			);


	return 0;
}







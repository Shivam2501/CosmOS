#include "rtc.h"
#include "terminal.h"
#include "general_operations.h"
#include "file.h"
#include "paging.h"


int32_t pid_tracker[max_num_processes];						//index into pid_tracker is pid-1

int32_t init_FD(){
	//am setting stdin and stdout fd blocks
	file_array_t stdin;
	stdin.flags = 1;
	//jump_table directly jumpts to function
	stdin.ops_table_ptr = (uint32_t)(&terminal_read);
	stdin.file_position = 0; 
	stdin.inode = NULL;

	file_array_t stdout;
	stdout.flags = 1; 
	stdout.ops_table_ptr = (uint32_t)(&terminal_write);
	stdout.file_position = 0; 
	stdout.inode = NULL;

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

	while(FD[index].flags ==1 && index <FD_SIZE){
		index++;
	}

	if(index == FD_SIZE)
		return -1;

	uint32_t (*op_open)();
	//file_array_t curr_file; 
													//set file in use

	dentry_t dentry_file_info;
	if(read_dentry_by_name((uint8_t*)filename, &dentry_file_info) == 0) {	//if file is valid 
		switch(dentry_file_info.file_type){
			case 0:
				{
					ops_table_t op_table;
					op_table.open = (uint32_t)(&rtc_open);
					op_table.close = (uint32_t)(&rtc_close);
					op_table.read = (uint32_t)(&rtc_read);
					op_table.write = (uint32_t)(&rtc_write);
		
					FD[index].ops_table_ptr = (uint32_t)(&op_table);
					
					op_open = (uint32_t (*)())(op_table.open);
					op_open(filename);
					break;
				}
			case 1:
				{
					ops_table_t op_table;										//creates jumptable for dir
					op_table.open = (uint32_t)(&dir_open);
					op_table.close = (uint32_t)(&dir_close);
					op_table.read = (uint32_t)(&dir_read);
					op_table.write = (uint32_t)(&dir_write);
			
					FD[index].ops_table_ptr = (uint32_t)(&op_table);			//sets ptr to jumptable in the struct for fd entry

					op_open = (uint32_t (*)())(op_table.open);
					op_open(filename);
					break;
				}
			case 2:
				{
					ops_table_t op_table;
					op_table.open = (uint32_t)(&fs_open);
					op_table.close = (uint32_t)(&fs_close);
					op_table.read = (uint32_t)(&fs_read);
					op_table.write = (uint32_t)(&fs_write);

					FD[index].ops_table_ptr = (uint32_t)(&op_table);

					op_open = (uint32_t (*)())(op_table.open);
					op_open(filename);
					break;
				}
		}
	} else {
		return -1;
	}

	return index; 
}


int32_t syscall_read(int32_t fd, void* buf, int32_t nbytes) {
	uint32_t (*op_read)();
	op_read = (uint32_t (*)()) ((FD[fd].ops_table_ptr).read); //???
	return op_read(fd, buf, nbytes);

	//return 0;
}

int32_t syscall_write(int32_t fd, const void* buf, int32_t nbytes) {
	uint32_t (*op_write)();
	op_write = (uint32_t (*)()) *(FD[fd].ops_table_ptr).write;
	return op_write(fd, buf, nbytes);
}

int32_t syscall_close(int32_t fd) {
	uint32_t (*op_close)();
	op_close = (uint32_t (*)()) *(FD[fd].ops_table_ptr).close;
	return op_close(fd, 0, 0);
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
	int i =0; 
	init_FD();																		//set stdin, stdout
	uint8_t* argument; 

	while(command[i]!= ' ')															//get first word
	{
		argument[i] = command[i];
		i++;
	}
	int32_t fd_index = syscall_open(argument);
	uint8_t buf[exe_buf_size];

	syscall_read(fd_index, buf, exe_buf_size);
	if(buf[0] != 0x7f || buf[1] != 0x45 || buf[2] != 0x4c || buf[3] != 0x46)		//if not executable
		return -1; 

	
	for(i = 0; i < max_num_processes; i++){											//find empty process 
		if(pid_tracker[i] == 0)
			break;
	}

	/*Paging*/
	page_directory[page_dir_entry] = (KERNEL_PROCESS_START + i*KERNEL_PROCESS_SIZE) | PS | READ_WRITE | PRESENT;
	loadPageDirectory((uint32_t*) page_directory);


	/*Load file in memory*/
	uint32_t* buffer = 0x800000;
	syscall_read(fd_index, buffer, buf_size);

	pid_tracker[i] = 1; 															//current process is being used
	PCB_t *pcb = (KERNEL_PROCESS_START - (i+1)*KERNEL_STACK_SIZE); 
	pcb->pid = i+1;
	
																					//save registers, cr3, flags in pcb
	asm volatile("                  \n												
			movl    %%cr3, %0;      \n
			movl 	%%esp, %1;		\n	
			movl    %%ebp, %2;		\n
			movl 	%%eflags, %3;   \n
			"
			: "=a"(pcb->tss.cr3), "=b"(pcb->tss.esp), "=c"(pcb->tss.ebp), "=d"(pcb->tss.eflags)
			:
			: "eax", "ebx", "ecx", "edx"	
			);

	pcb->tss.esp0 = KERNEL_PROCESS_START - i*KERNEL_STACK_SIZE;	
	pcb->pcb_fd = FD;					



	return 0;
}







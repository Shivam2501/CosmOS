#include "rtc.h"
#include "terminal.h"
#include "general_operations.h"
#include "file.h"

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
					rtc_ops_t op_table;
					op_table.open_rtc = (uint32_t)(&rtc_open);
					op_table.close_rtc = (uint32_t)(&rtc_close);
					op_table.read_rtc = (uint32_t)(&rtc_read);
					op_table.write_rtc = (uint32_t)(&rtc_write);
		
					FD[index].ops_table_ptr = (uint32_t)(&op_table);
					
					op_open = (uint32_t (*)())(op_table.open_rtc);
					op_open(filename);
					break;
				}
			case 1:
				{
					dir_ops_t op_table;											//creates jumptable for dir
					op_table.open_dir = (uint32_t)(&dir_open);
					op_table.close_dir = (uint32_t)(&dir_close);
					op_table.read_dir = (uint32_t)(&dir_read);
					op_table.write_dir = (uint32_t)(&dir_write);
			
					FD[index].ops_table_ptr = (uint32_t)(&op_table);						//sets ptr to jumptable in the struct for fd entry

					op_open = (uint32_t (*)())(op_table.open_dir);
					op_open(filename);
					break;
				}
			case 2:
				{
					file_ops_t op_table;
					op_table.open_file = (uint32_t)(&fs_open);
					op_table.close_file = (uint32_t)(&fs_close);
					op_table.read_file = (uint32_t)(&fs_read);
					op_table.write_file = (uint32_t)(&fs_write);

					FD[index].ops_table_ptr = (uint32_t)(&op_table);

					op_open = (uint32_t (*)())(op_table.open_file);
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
	op_read = (uint32_t (*)())(FD[fd].ops_table_ptr);
	return op_read(fd, buf, nbytes, 2);

	//return 0;
}

int32_t syscall_write(int32_t fd, const void* buf, int32_t nbytes) {
	uint32_t (*op_write)();
	op_write = (uint32_t (*)())(FD[fd].ops_table_ptr);
	return op_write(fd, buf, nbytes, 3);
}

int32_t syscall_close(int32_t fd) {
	uint32_t (*op_close)();
	op_close = (uint32_t (*)())(FD[fd].ops_table_ptr);
	return op_close(fd, 0, 0, 1);
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
	return 0;
}







#include "rtc.h"
#include "terminal.h"

file_array_t FD[8]; 

int32_t init_FD(){
	//am setting stdin and stdout fd blocks
	file_array_t stdin;
	stdin.flags = 1;
	//jump table directly jumpts to function
	stdin.ops_table_ptr = &terminal_read;
	stdin.file_position = 0; 
	stdin.inode = NULL;

	file_array_t stdout;
	stdout.flags = 1; 
	stdout.ops_table_ptr = &terminal_write;
	stdout.file_position = 0; 
	stdout.inode = NULL;
}

/*
 * general_open
 *   DESCRIPTION: Open the file
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success, -1 on failure
 */ 
int32_t general_open(const uint8_t* filename) {
	int index = DEFAULT_FD;

	while(FD[index].flags ==1 && index <FD_SIZE){
		index++;
	}

	if(index == FD_SIZE)
		return -1;

	file_array_t curr_file; 
													//set file in use

	dentry_t dentry_file_info;
	if(read_dentry_by_name((uint8_t*)filename, &dentry_file_info) == 0) {	//if file is valid 
		switch(dentry_file_info.file_type){
			case 0:
				curr_file.file_position = 0; //???
				curr_file.inode = NULL;
				curr_file.flags = 1;	
				curr_file.ops_table_ptr = &rtc_ops;
				rtc_open(filename);
				break;
			case 1:
				curr_file.file_position = 0; //???
				curr_file.ops_table_ptr = &dir_ops;
				curr_file.inode = NULL;
				curr_file.flags = 1;	
				break;
			case 2:
				fs_open(filename);
				break;
		}
	} else {
		return -1;
	}

	return index; 
}


int32_t general_read(int32_t fd, void* buf, int32_t nbytes) {
	uint32_t (*op_read)();
	op_read = FD[fd].ops_table_ptr;
	return op_read(fd, buf, nbytes, 2);

	//return 0;
}

int32_t general_write(int32_t fd, const void* buf, int32_t nbytes) {
	uint32_t (*op_write)();
	op_write = FD[fd].ops_table_ptr;
	return op_write(fd, buf, nbytes, 3);
}

int32_t general_close(int32_t fd) {
	uint32_t (*op_close)();
	op_close = FD[fd].ops_table_ptr;
	return op_close(fd, 0, 0, 1);
}

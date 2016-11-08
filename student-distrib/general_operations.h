#ifndef _GENERAL_OPERATIONS_H
#define _GENERAL_OPERATIONS_H

typedef struct file_array {
    uint32_t 	ops_table_ptr;
    uint32_t*   inode; 
    uint32_t	file_position;   
    uint32_t 	flags;                  
} file_array_t;

typedef struct rtc_ops{
	uint32_t 	open_rtc;
	uint32_t 	close_rtc;
	uint32_t 	read_rtc;
	uint32_t 	write_rtc;
} rtc_ops_t;

typedef struct file_ops{
	uint32_t 	open_file;
	uint32_t 	close_file;
	uint32_t 	read_file;
	uint32_t 	write_file;
} file_ops_t;

typedef struct dir_ops{
	uint32_t 	open_dir;
	uint32_t 	close_dir;
	uint32_t 	read_dir;
	uint32_t 	write_dir;
} dir_ops_t;

file_array_t FD[8]; 


int32_t general_getargs (uint8_t* buf, int32_t nbytes);
int32_t general_vidmap (uint8_t** screen_start);
int32_t general_set_handler (int32_t signum, void* handler_address);
int32_t general_sigreturn (void);
int32_t general_halt (uint8_t status);
int32_t general_execute (const uint8_t* command);
int32_t general_read (int32_t fd, void* buf, int32_t nbytes);
int32_t general_write (int32_t fd, const void* buf, int32_t nbytes);
int32_t general_open (const uint8_t* filename);
int32_t general_close (int32_t fd);



#endif

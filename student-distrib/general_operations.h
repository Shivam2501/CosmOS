#ifndef _GENERAL_OPERATIONS_H
#define _GENERAL_OPERATIONS_H
#define exe_buf_size 	4
#define page_dir_entry  0x20
#define	max_num_processes   6
#define KERNEL_PROCESS_START	0x800000
#define KERNEL_PROCESS_SIZE		0x400000
#define KERNEL_STACK_SIZE		0x2000
#define buf_size 			4
#include "x86_desc.h"

typedef struct file_array {
    uint32_t 	ops_table_ptr;
    uint32_t*   inode; 
    uint32_t	file_position;   
    uint32_t 	flags;                  
} file_array_t;

typedef struct ops_table{
	uint32_t 	open;
	uint32_t 	close;
	uint32_t 	read;
	uint32_t 	write;
} ops_table_t;

typedef struct PCB{
	uint32_t 	pid;
	tss_t 		tss;
	file_array_t pcb_fd[8];
} PCB_t;

file_array_t FD[8]; 


int32_t syscall_getargs (uint8_t* buf, int32_t nbytes);
int32_t syscall_vidmap (uint8_t** screen_start);
int32_t syscall_set_handler (int32_t signum, void* handler_address);
int32_t syscall_sigreturn (void);
int32_t syscall_halt (uint8_t status);
int32_t syscall_execute (const uint8_t* command);
int32_t syscall_read (int32_t fd, void* buf, int32_t nbytes);
int32_t syscall_write (int32_t fd, const void* buf, int32_t nbytes);
int32_t syscall_open (const uint8_t* filename);
int32_t syscall_close (int32_t fd);



#endif

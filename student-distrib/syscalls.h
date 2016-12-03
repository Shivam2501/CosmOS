#ifndef _SYSCALLS_H
#define _SYSCALLS_H

#define EXE_BUF_SIZE		 	4
#define PAGE_DIR_ENTRY			0x20
#define	MAX_NUM_PROCESS		   	6
#define USER_PROGRAM_START		0x08000000
#define VIRTUAL_ADDRESS_PROGRAM	0x08048000
#define KERNEL_PROCESS_START	0x800000
#define KERNEL_PROCESS_SIZE		0x400000
#define KERNEL_STACK_SIZE		0x2000
#define BUF_SIZE 				4
#define EIP_READ_OFFSET			24
#define EFLAGS_VALUE			0x00000200
#define ESP_VALUE				0x83FFFFC
#define MAX_BUFFER_SIZE		 	128
#define MAX_OPEN_FILES			8
#define FILE_SIZE  				0x3FFFFC
#define PAGE_ALIGNMENT			4
#define _132MB					0x8400000
#define VIDEO_MEM 				0xB8000

#include "x86_desc.h"

typedef struct ops_table{
	int32_t (*open) (const uint8_t* filename);
	int32_t (*close) (int32_t fd);
	int32_t (*read) (int32_t fd, void* buf, int32_t nbytes);
	int32_t (*write) (int32_t fd, const void* buf, int32_t nbytes);
} ops_table_t;

typedef struct file_array{
    ops_table_t 	ops_table_ptr;
    uint32_t	   	inode; 
    uint32_t		file_position;   
    uint32_t 		flags;                  
} file_array_t;
 
typedef struct PCB{
	uint32_t 			pid;
	tss_t 				tss;
	file_array_t 		FD[MAX_OPEN_FILES];
	uint32_t			parent_ptr;
	uint8_t 			arguments[MAX_BUFFER_SIZE];
} PCB_t;

#include "i8259.h"
#include "lib.h"
#include "paging.h"
#include "file.h"
#include "rtc.h"
#include "terminal.h"
#include "keyboard.h"

// extern PCB_t PCB_t;

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
int32_t syscall_fail (void);
int get_available_pid();

#endif

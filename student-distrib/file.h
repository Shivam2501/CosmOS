#ifndef _FILE_H
#define _FILE_H

#include "types.h"
#include "multiboot.h"
#include "lib.h"

#define NUMBER_DIR_ENTRIES 			1
#define NUMBER_INODES				1
#define NUMBER_DATA_BLOCKS			1
#define RESERVED0 					13
#define FILE_NAME_LENGTH 			8
#define	FILE_TYPE_LENGTH			1
#define	DIR_ENTRY_LENGTH 			16
#define BLOCK_SIZE					1024
#define BLOCK_SIZE_FOUR				4096

#define SIZE_DATA_BLOCK				4
#define NAME_SIZE					32

typedef struct dentry_header {
    uint8_t 	file_name[32];
    uint32_t    file_type; 
    uint32_t	inode;                     
} dentry_t;

typedef struct boot_header {
	uint32_t 	dir_entries;
	uint32_t    inodes;
	uint32_t    data_blocks;
} boot_t;

/* Helper function to set the start address of the file system*/
int get_file_system_start(unsigned int mod_start);

/* Helper functions to assist in system calls to read file system*/
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

/* System calls */
int32_t fs_open(const uint8_t* filename);
int32_t fs_read(int32_t fd, void* buf, int32_t nbytes);
int32_t dir_read(int32_t fd, void* buf, int32_t nbytes);
int32_t fs_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t fs_close(int32_t fd);

/* Helper function to read the size of the file */
int32_t fs_size(int32_t fd, void* buf, int32_t nbytes);

#endif

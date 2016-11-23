#ifndef _FILE_H
#define _FILE_H

#include "types.h"
#include "multiboot.h"
#include "lib.h"
#include "general_operations.h"

#define NUMBER_DIR_ENTRIES 			1
#define NUMBER_INODES				1
#define NUMBER_DATA_BLOCKS			1
#define reserved0 					13
#define file_name_length 			8
#define	file_type_length			1
#define	dir_entry_length 			16
#define BLOCK_SIZE					1024
#define BLOCK_SIZE_FOUR				4096
#define	FD_SIZE						8
#define DEFAULT_FD                  2
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


int get_file_system_start(unsigned int mod_start);

int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry);
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

int32_t fs_open(const uint8_t* filename);
int32_t fs_read(int32_t fd, void* buf, int32_t nbytes);
int32_t fs_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t fs_size(int32_t fd, void* buf, int32_t nbytes);
int32_t fs_close(int32_t fd);

int32_t dir_read(int32_t fd, void* buf, int32_t nbytes);
int32_t dir_write(int32_t fd, const void* buf, int32_t nbytes);
int32_t dir_open(const uint8_t* filename);
int32_t dir_close(int32_t fd);

#endif

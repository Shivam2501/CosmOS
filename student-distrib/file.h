#include "types.h"
#include "multiboot.h"
#include "lib.h"

#define NUMBER_DIR_ENTRIES 			1
#define NUMBER_INODES				1
#define NUMBER_DATA_BLOCKS			1
#define reserved0 					13
#define file_name_length 			8
#define	file_type_length			1
#define	dir_entry_length 			16

#define INODE_LENGTH 				4096 //bytes in kilobyte


typedef struct dentry_header {
    uint8_t 	file_name[32];
    uint32_t    file_type; 
    uint32_t	inode;
    int 		reserved;                       
} dentry_t;

typedef struct boot_header {
	uint32_t 	dir_entries;
	uint32_t    inodes;
	uint32_t    data_blocks;
} boot_t;

int get_file_system_start(unsigned int mod_start);
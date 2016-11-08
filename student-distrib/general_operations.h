#ifndef _GENERAL_OPERATIONS_H
#define _GENERAL_OPERATIONS_H

typedef struct file_array {
    uint32_t 	ops_table_ptr;
    uint32_t    inode; 
    uint32_t	file_position;   
    uint32_t 	flags;                  
} file_array_t;


#endif

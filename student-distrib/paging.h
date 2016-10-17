/* paging.h - Defines for paging handler
 * vim:ts=4 noexpandtab
 */

#ifndef _PAGING_H
#define _PAGING_H

#include "types.h"

// video memory address
#define VIDEO_MEMORY_ADDRESS	0xB8000

// kernel memory address
#define KERNEL_MEMORY_ADDRESS	0x400000

// page table address for video memory
#define INDEX_TO_TABLE 			0xB8

/* Initialize the paging */
extern void init_paging();

/* load page directory */
extern void loadPageDirectory(unsigned int*);

/* enable paging */
extern void enablePaging();

#endif /* _PAGING_H */

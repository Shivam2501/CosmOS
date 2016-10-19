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

#define SIZE_DIR_TABLE			1024

#define KERNEL_INDEX			1
#define VIDEO_INDEX				0

#define ALIGN					4096

#define READ_WRITE				0x2
#define PRESENT					0x3
#define PS 						0x83

/* Initialize the paging */
extern void init_paging();

/* load page directory */
extern void loadPageDirectory(unsigned int*);

/* enable paging */
extern void enablePaging();

#endif /* _PAGING_H */

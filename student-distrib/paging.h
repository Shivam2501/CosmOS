/* paging.h - Defines for paging handler
 * vim:ts=4 noexpandtab
 */

#ifndef _PAGING_H
#define _PAGING_H

#include "types.h"

#define VIDEO_MEMORY_ADDRESS	0xB8000
#define KERNEL_MEMORY_ADDRESS	0x400000

extern void init_paging();

extern void loadPageDirectory(unsigned int*);
extern void enablePaging();

#endif /* _PAGING_H */

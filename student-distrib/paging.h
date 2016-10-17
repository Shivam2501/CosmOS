/* paging.h - Defines for paging handler
 * vim:ts=4 noexpandtab
 */

#ifndef _PAGING_H
#define _PAGING_H

#define VIDEO_MEMORY_ADDRESS	0xB8000
#define KERNEL_MEMORY_ADDRESS	0x400000

extern void init_paging();

#endif /* _PAGING_H */

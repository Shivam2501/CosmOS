#ifndef _MEMORY_ALLOCATOR_H
#define _MEMORY_ALLOCATOR_H

#include "lib.h"
#include "paging.h"

#define KERNEL_HEAP_BASE		0x800000 	//8MB
#define KERNEL_HEAP_MAX			0x7FFFFFC 	//128MB - 4Bits
#define KERNEL_HEAP_PHYSICAL	0x3C00000   //60MB

#define USER_HEAP_BASE			0x8800000 	//136MB
#define USER_HEAP_MAX			0x100000000 //4GB
#define USER_HEAP_PHYSICAL		0xB400000   //60MB + (128MB - 8MB) = 180MB			

#define MEMLIST					11
	
#define MIN_SIZE				1
#define	MAX_SIZE				4096

#define _4MB_OFFSET				0x400000

typedef struct mem_page mem_page;

struct mem_page {
	uint32_t	count_free;
	void* 		addr;
	mem_page*	next;
	uint32_t	block_number;
};

typedef struct mem_list {
	mem_page*	lists[MEMLIST];
	uint32_t	base_address;
	uint32_t	max_address;
} mem_list;

int32_t user_mem_init();
int32_t kernel_mem_init();

/* Helper Functions */
uint32_t next_power_of_2(uint32_t num);
uint32_t power_of_2(uint32_t num);
void* buddy_block(void* address, uint32_t size);

/* Main Functions */
void* kmalloc(uint32_t size);
void kfree(void* address);

#endif

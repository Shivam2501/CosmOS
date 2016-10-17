#include "paging.h"

void init_paging() {
	uint32_t page_directory[1024] __attribute__((aligned(4096)));
	uint32_t page_table[1024] __attribute__((aligned(4096)));
	
	uint16_t index_to_directory, index_to_table;

	/* Set each page directory entry to not present */
	int i;
	for(i =0; i < 1024; i++){
		// enable read/write
		page_directory[i] = 0x2;
		page_table[i] = 0x2;
	}

	//page directory for 4MB pages
	page_directory[1] = KERNEL_MEMORY_ADDRESS | 0x83;

	//page directory for video memory
	index_to_table = (VIDEO_MEMORY_ADDRESS & 0x3FF000) >> 12;

	//enable video memory
	page_table[index_to_table] = VIDEO_MEMORY_ADDRESS | 0x3;

	//page directory index points to page table and is set to present
	page_directory[0] = page_table | 0x3;

	/* enabling paging */
	asm volatile("						\n\
		 movl $page_directory, %%eax  	\n\
		 movl %%eax, %%cr3 				\n\
		 movl %%cr4, %%eax				\n\
		 orl $0x00000010, %%eax 		\n\
		 movl %%eax, %%cr4 				\n\
		 movl %%cr0, %%eax				\n\
		 orl $0x80000000, %%eax			\n\
		 movl %%eax, %%cr0				\n\
		"
		: : : "eax", "cc");
}
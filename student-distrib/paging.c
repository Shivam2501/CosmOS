#include "paging.h"

void init_paging() {
	uint32_t page_directory[1024] __attribute__((aligned(4096)));
	uint32_t page_table[1024] __attribute__((aligned(4096)));
	
	uint16_t index_to_directory, index_to_table;

	/* Set each page directory entry to not present */
	int i;
	for(i =0; i < 1024; i++){
		// enable read/write
		page_directory[i] = 0x00000002;
		page_table[i] = 0x00000002;
	}

	//page directory for 4MB pages
	index_to_directory = (KERNEL_MEMORY_ADDRESS & 0xFFC00000) >> 22;
	page_directory[index_to_directory] = KERNEL_MEMORY_ADDRESS | 0x00000083;

	//page directory for video memory
	index_to_directory = (VIDEO_MEMORY_ADDRESS & 0xFFC00000) >> 22;
	index_to_table = (VIDEO_MEMORY_ADDRESS & 0x3FF000) >> 12;

	page_table[index_to_table] = VIDEO_MEMORY_ADDRESS | 0x00000003;

	page_directory[index_to_directory] = page_table | 0x00000003;

	/* enabling paging */
	asm volatile("						\n\
		 movl $page_directory, %%eax  	\n\
		 movl %%eax, cr3 				\n\
		 movl %%cr4, %%eax				\n\
		 orl $0x00000010, %%eax 		\n\
		 movl %%eax, cr4 				\n\
		 movl %%cr0, %%eax				\n\
		 orl $0x80000000, %%eax			\n\
		 movl %%eax, %%cr0				\n\
		");
}

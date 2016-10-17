#include "paging.h"

//page directory array
uint32_t page_directory[1024] __attribute__((aligned(4096)));
//page table array
uint32_t page_table[1024] __attribute__((aligned(4096)));


/*
 * init_paging
 *   DESCRIPTION: Initialize the paging. 
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */ 
void init_paging() {
	/* Set each page directory entry to not present */
	int i;
	for(i=0; i < 1024; i++){
		// enable read/write
		page_directory[i] = 0x2;
		page_table[i] = 0x2;
	}

	//page directory for 4MB pages
	page_directory[1] = KERNEL_MEMORY_ADDRESS | 0x83;

	//enable video memory
	page_table[INDEX_TO_TABLE] = VIDEO_MEMORY_ADDRESS | 0x3;

	//page directory index points to page table and is set to present
	page_directory[0] = (uint32_t)page_table | 0x3;

	loadPageDirectory((uint32_t)page_directory);
	enablePaging();
}

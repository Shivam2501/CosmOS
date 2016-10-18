#include "paging.h"

//page directory array
uint32_t page_directory[SIZE_DIR_TABLE] __attribute__((aligned(ALIGN)));
//page table array
uint32_t page_table[SIZE_DIR_TABLE] __attribute__((aligned(ALIGN)));


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
	for(i=0; i < SIZE_DIR_TABLE; i++){
		// enable read/write (0X2)
		page_directory[i] = 0x2;
		page_table[i] = 0x2;
	}

	//page directory for 4MB pages (0x83: Enable PS, Present and Read/Write)
	page_directory[KERNEL_INDEX] = KERNEL_MEMORY_ADDRESS | 0x83;

	//enable video memory (0x3: Present and Read/Write)
	page_table[INDEX_TO_TABLE] = VIDEO_MEMORY_ADDRESS | 0x3;

	//page directory index points to page table and is set to present
	// (0x3: Present and Read/Write)
	page_directory[VIDEO_INDEX] = (uint32_t)page_table | 0x3;

	/* load page directory */
	loadPageDirectory((uint32_t *)page_directory);

	/* Enable page directory */
	enablePaging();
}

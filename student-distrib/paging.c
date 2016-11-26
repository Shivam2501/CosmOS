#include "paging.h"

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
		// enable read/write (0x2)
		page_directory[i] = READ_WRITE;
		page_table[i] = READ_WRITE;
	}

	//page directory for 4MB pages (0x83: Enable PS, Present and Read/Write)
	page_directory[KERNEL_INDEX] = KERNEL_MEMORY_ADDRESS | PS | READ_WRITE | PRESENT;

	//enable video memory (0x3: Present and Read/Write)
	page_table[INDEX_TO_TABLE] = VIDEO_MEMORY_ADDRESS | READ_WRITE | PRESENT;
	page_table[INDEX_VGA] = VGA_ADDRESS | READ_WRITE | PRESENT;

	//page directory index points to page table and is set to present
	// (0x3: Present and Read/Write)
	page_directory[VIDEO_INDEX] = (uint32_t)page_table | READ_WRITE | PRESENT;

	/* load page directory */
	loadPageDirectory((uint32_t *)page_directory);

	/* Enable page directory */
	enablePaging();
}

/*
 * add_paging
 *   DESCRIPTION: map paging for process execute. 
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */ 
void add_paging(uint32_t virtual, uint32_t physical) {

	page_directory[virtual] = physical | PS | USER | READ_WRITE | PRESENT  ;
	
	//tlb flush
	asm volatile("				\n\
		movl	%%cr3,%%eax		\n\
		movl	%%eax,%%cr3		\n\
		"
		:
		:
		: "eax"
		);
}

/*
 * kernel_add_paging
 *   DESCRIPTION: Initialize the paging for kernel heap memory. 
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */ 
void kernel_add_paging(uint32_t virtual_start, uint32_t virtual_end, uint32_t physical) {

	int i, index;
	for(i = virtual_start; i < virtual_end; i = i + _4MB_OFFSET) {
		//calculate page directory index of 4MB page
		index = (i >> 22);
		//page directory for 4MB pages (0x83: Enable PS, Present and Read/Write)
		page_directory[index] = physical | PS | READ_WRITE | PRESENT;
		physical = physical + _4MB_OFFSET;
	}

}

/*
 * user_add_paging
 *   DESCRIPTION: Initialize the paging for user heap memory. 
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */ 
void user_add_paging(uint32_t virtual_start, uint32_t virtual_end, uint32_t physical) {

	int i, index;
	for(i = virtual_start; i < virtual_end; i = i + _4MB_OFFSET) {
		//calculate page directory index of 4MB page
		index = (i >> 22);
		//page directory for 4MB pages (0x83: Enable PS, Present and Read/Write)
		page_directory[index] = physical | PS | USER | READ_WRITE | PRESENT;
		physical = physical + _4MB_OFFSET;
	}

}

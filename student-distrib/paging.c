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

	for(i=0; i < SIZE_DIR_TABLE; i++){
		page_table_video[i] = READ_WRITE;
	}

	//page directory for 4MB pages (0x83: Enable PS, Present and Read/Write)
	page_directory[KERNEL_INDEX] = KERNEL_MEMORY_ADDRESS | PS | READ_WRITE | PRESENT;

	//enable video memory (0x3: Present and Read/Write)
	page_table[INDEX_TO_TABLE] = VIDEO_MEMORY_ADDRESS | READ_WRITE | PRESENT;

	//enable paging for mode 13
	int physical = VGA_ADDRESS;
	for(i = INDEX_VGA; i < INDEX_VGA + 16; i++) {
		page_table[i] = physical | READ_WRITE | PRESENT;
		physical += _4KB_OFFSET;
	}

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
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */ 
void add_paging(uint32_t virtual, uint32_t physical) {
	page_directory[virtual] = physical | PS | USER | READ_WRITE | PRESENT  ;
	tlb_flush();
}

/*
 * add_paging_4kb
 *   DESCRIPTION: Map text mode video to user space
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */ 
void add_paging_4kb(uint32_t virtual, uint32_t physical) {
	/* Set each page directory entry to not present */
	int index;

	//bitmask by 0x3FF and shift by 12 to get middle 10 bits
	index = (virtual >> 12) & 0x3FF;
	//enable video memory (0x3: Present and Read/Write)
	page_table_video[index] = physical | USER | READ_WRITE | PRESENT;

	index = (virtual >> 22);
	//page directory index points to page table and is set to present
	// (0x3: Present and Read/Write)
	page_directory[index] = (uint32_t)page_table_video | USER | READ_WRITE | PRESENT;
	tlb_flush();
}

/*
 * tlb_flush
 *   DESCRIPTION: Flush the TLB 
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 */ 
void tlb_flush(void) {
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

#include "memory_allocator.h"

mem_list kernel_heap;
mem_list user_heap;

/*
 * user_mem_init
 *   DESCRIPTION: Setup the heap memory for user space
 *   INPUTS: none
 *   OUTPUTS:
 *   RETURN VALUE: 0
 */ 
/*
int32_t user_mem_init() {
	//mark the heap space as present for user heap memory
	user_add_paging(USER_HEAP_BASE, USER_HEAP_MAX, USER_HEAP_PHYSICAL);

	//fill the kernel heap
	user_heap.base_address = USER_HEAP_BASE;
	user_heap.max_address = USER_HEAP_MAX;

	int i;

	//initialize the list with 4MB pages
	int count = 1;
	for(i = USER_HEAP_BASE; i < USER_HEAP_MAX; i = i + 4MB_OFFSET) {
		mem_page *new_page = (mem_page*)i;

		new_page->count_free = count; //store the number of free page when this node is the head of the list
		new_page->addr = i; //base address which the node points to

		//check if the list is empty, if yes then list points to NULL otherwise to the current head
		if(user_heap.lists[MEMLIST - 1] == 0) {
			new_page->next = NULL;
		} else {
			new_page->next = user_heap.lists[MEMLIST - 1];
		}

		new_page->block_number = MEMLIST - 1;
		user_heap.lists[MEMLIST - 1] = new_page;

		count++;
	}

}*/

/*
 * mem_init
 *   DESCRIPTION: Setup the heap memory for kernel space
 *   INPUTS: none
 *   OUTPUTS:
 *   RETURN VALUE: 0
 */ 
int32_t kernel_mem_init() {
	//mark the heap space as present for kernel heap memory
	kernel_add_paging(KERNEL_HEAP_BASE, KERNEL_HEAP_MAX, KERNEL_HEAP_PHYSICAL);

	//fill the kernel heap
	kernel_heap.base_address = KERNEL_HEAP_BASE;
	kernel_heap.max_address = KERNEL_HEAP_MAX;

	int i;

	//initialize the list with 4MB pages
	int count = 1;
	for(i = KERNEL_HEAP_BASE; i < KERNEL_HEAP_MAX; i = i + _4MB_OFFSET) {
		mem_page *new_page = (mem_page*)i;

		new_page->count_free = count; //store the number of free page when this node is the head of the list
		new_page->addr = new_page; //base address which the node points to

		//check if the list is empty, if yes then list points to NULL otherwise to the current head
		if(kernel_heap.lists[MEMLIST - 1] == 0) {
			new_page->next = NULL;
		} else {
			new_page->next = kernel_heap.lists[MEMLIST - 1];
		}

		new_page->block_number = MEMLIST - 1;
		kernel_heap.lists[MEMLIST - 1] = new_page;

		count++;
	}

	return 1;
}

/*
 * next_power_of_2
 *   DESCRIPTION: Round up tp the next highest power of 2
 *   INPUTS: number to be rounded up
 *   OUTPUTS: 
 *   RETURN VALUE: power of 2
 */
uint32_t next_power_of_2(uint32_t num) {
	//round up to the next highest power of 2
	num--;
    num |= num >> 1;
    num |= num >> 2;
    num |= num >> 4;
    num |= num >> 8;
    num |= num >> 16;
    num++;

    return num;
}

/*
 * power_of_2
 *   DESCRIPTION: Find the power of 2
 *   INPUTS: power
 *   OUTPUTS: 
 *   RETURN VALUE: 2^num
 */
uint32_t power_of_2(uint32_t num) {

	uint32_t size = 1;
    while(num != 0) {
        size = size * 2;
        num--;
    }
    return size;
}

/*
 * buddy_block
 *   DESCRIPTION: Find the buddy of the block
 *   INPUTS: address for which buddy is being found and size of the block
 *   OUTPUTS: 
 *   RETURN VALUE: address of the buddy block
 */ 
void* buddy_block(void* address, uint32_t size) {
	int block_sign = (int)(address - kernel_heap.base_address) / (size*MAX_SIZE);
	if(block_sign == 0 || block_sign % 2 == 0) {
		//even signedness
		return (void*)(address + (size*MAX_SIZE));
	} else {
		//odd signedness
		return (void*)(address - (size*MAX_SIZE));
	}
}

/*
 * buddy_allocator
 *   DESCRIPTION: Allocates memory as per requested size
 *   INPUTS: size 
 *   OUTPUTS: 
 *   RETURN VALUE: address where memory is allocated
 */ 
void* kmalloc(uint32_t size) {

	//node information is stored at the beginning
	size = size + sizeof(mem_page);
	//round up to the next highest power of 2
	size = next_power_of_2(size);

    //compare the bounds of size
    if( size < MIN_SIZE || size > MAX_SIZE) {
    	return NULL;
    }

    //each page frame stores 4 bytes
    size /= 4;

    //find log base 2 of the size to find the list number
    uint32_t temp = size, l = 0;
    while (temp >>= 1) { 
        ++l; 
    }

    temp = l;
    //find the list with a free block
    while(kernel_heap.lists[temp] == 0) {
    	temp++;
    	//if no free page of the required size
    	if(temp >= MEMLIST)
    		return NULL;
    }

    uint32_t block_size, new_block_size;
    //split blocks until the next index is equal to the required index
    while(temp != l) {
    	//the block to be assigned 
    	mem_page *curr_page = kernel_heap.lists[temp];
    	//remove the curr_page from the free list
    	kernel_heap.lists[temp] = curr_page->next;

    	//calculate the block_size for each split
        block_size = power_of_2(curr_page->block_number);

        //take the current block and split into into two blocks of the next smaller size
        new_block_size = block_size / 2;

        //second page will be present at current address + number of bytes in the new block size
       	mem_page *second_page = (mem_page*)(buddy_block(curr_page->addr, new_block_size));
       	second_page->addr = second_page;

       	//check if the list is empty, if yes then list points to NULL otherwise to the current head
		if(kernel_heap.lists[temp - 1] == 0) {
			second_page->next = NULL;
			second_page->count_free = 1;
		} else {
			second_page->next = kernel_heap.lists[temp - 1];
			second_page->count_free = kernel_heap.lists[temp - 1]->count_free + 1;
		}

		second_page->block_number = temp - 1;
		kernel_heap.lists[temp - 1] = second_page;

		curr_page->next = kernel_heap.lists[temp - 1];
		curr_page->count_free = kernel_heap.lists[temp - 1]->count_free + 1;
		curr_page->block_number = temp - 1;
		kernel_heap.lists[temp - 1] = curr_page;

    	temp--;
    }

    //page which is returned and allocated for completing user request
    mem_page *alloc_page = kernel_heap.lists[temp];
    //remove the curr_page from the free list
    kernel_heap.lists[temp] = alloc_page->next;

   	return (void*)((alloc_page->addr) + sizeof(mem_page));
}

/*
 * buddy_deallocator
 *   DESCRIPTION: Deallocate memory 
 *   INPUTS: address  
 *   OUTPUTS: 
 *   RETURN VALUE: none
 */ 
void kfree(void* address) {
	mem_page *curr_page = (mem_page*)(address - sizeof(mem_page));

	//check the bounds of the block_number
	if(curr_page->block_number < 0 || curr_page->block_number >= MEMLIST)
		return;

	int temp = curr_page->block_number;
	uint32_t block_size;
	//try to merge buddy block if present until the biggest block is reached
	while(temp < MEMLIST - 1) {
		//calculate the block_size for each split
        block_size = power_of_2(temp);

		//find the buddy block
		mem_page *buddy_page = (mem_page*)(buddy_block(curr_page->addr, block_size));
       	buddy_page->addr = buddy_page;

       	//iterate the list to see if the buddy block is present
       	mem_page *head = kernel_heap.lists[temp];
       	while(head) {
       		if(head->addr == buddy_page->addr)
       			break;
       		head = head->next;
       	}
       	//if the buddy block is not present
       	if(head == 0)
       		break;

       	if(curr_page->addr > buddy_page->addr){
       		curr_page = buddy_page;
       	}

       	//merge into current page
       	curr_page->block_number = temp + 1;

       	//check if buddy page is head of the list
       	if(kernel_heap.lists[temp]->addr == buddy_page->addr) {
       		//remove from list directly
       		kernel_heap.lists[temp] = kernel_heap.lists[temp]->next;
       	} else {
       		//parse the list again and get the previous block to buddy block
       		head = kernel_heap.lists[temp];
       		while(head->next) {
       			head->count_free--;
       			if(head->next->addr == buddy_page->addr){
       				head->next = head->next->next;
       				break;
       			}
       			head = head->next;
       		}
       	}

       	temp = curr_page->block_number;
	}

	//insert the block into current list
	//check if the list is empty, if yes then list points to NULL otherwise to the current head
	if(kernel_heap.lists[curr_page->block_number] == 0) {
		curr_page->next = NULL;
		curr_page->count_free = 1;
	} else {
		curr_page->next = kernel_heap.lists[curr_page->block_number];
		curr_page->count_free = kernel_heap.lists[curr_page->block_number]->count_free + 1;
	}

	//add the block to the free list
	kernel_heap.lists[curr_page->block_number] = curr_page;

}

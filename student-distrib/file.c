#include "file.h"

uint32_t *start_addr, *dir_entry_start; 
uint32_t dir_read_counter = 0;


boot_t boot_info;

/*
 * get_file_system_start
 *   DESCRIPTION: Creates the global pointer to the start address and directory entry start, and also saves relevant information 
 *   INPUTS: pointer to start address
 *   OUTPUTS: none
 *   RETURN VALUE: 0
 */ 
int get_file_system_start(unsigned int mod_start){
	start_addr = (uint32_t*) mod_start;

	//need to add beginning address + everything in the boot block 
	dir_entry_start = (uint32_t*) (start_addr + 
				NUMBER_DIR_ENTRIES + NUMBER_INODES + NUMBER_DATA_BLOCKS + reserved0);

	//puts number of directory entries, number of inodes, and number of data blocks in a struct to be more accessible
	memcpy(&(boot_info.dir_entries), start_addr, SIZE_DATA_BLOCK);
	memcpy(&(boot_info.inodes), start_addr + NUMBER_DIR_ENTRIES, SIZE_DATA_BLOCK);
	memcpy(&(boot_info.data_blocks), start_addr + NUMBER_DIR_ENTRIES + NUMBER_INODES, SIZE_DATA_BLOCK);

	return 0;
}

/*
 * read_dentry_by_index
 *   DESCRIPTION: If given an index, gather relevant information (file name, type, inode#) and save into dentry
 *   INPUTS: index and reference to dentry struct to populate
 *   OUTPUTS: none
 *   RETURN VALUE: 0 if successful, -1 otherwise
 */ 
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry){
	
	//check if index is invalid, if it is greater than the max number possible
	if(index >= boot_info.dir_entries){
		return -1; 
	}

	//memory starts at block 0, then we skip over until specific block needed
 	uint32_t* curr_start = dir_entry_start + index*dir_entry_length;

 	memcpy(&(dentry->file_name), curr_start, NAME_SIZE);
 	memcpy(&(dentry->file_type), curr_start + file_name_length, SIZE_DATA_BLOCK);
 	memcpy(&(dentry->inode), curr_start + file_name_length + file_type_length, SIZE_DATA_BLOCK);

	return 0; 
}

/*
 * read_dentry_by_name
 *   DESCRIPTION: If given a name, populate dentry with relevant information (file name, type, inode)
 *   INPUTS: file name and reference to struct to be populated
 *   OUTPUTS: none
 *   RETURN VALUE: 0 if success, -1 otherwise
 */ 
int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry){

	//if the filename provided is greater than max size
	if(strlen((int8_t*)fname) > NAME_SIZE)
		return -1; 

	uint32_t i;
	uint32_t* curr_start; 

	for(i = 0; i < boot_info.dir_entries; i++){
		uint8_t filename[NAME_SIZE];
		memcpy(filename, dir_entry_start + (i*dir_entry_length), NAME_SIZE);

		//compare to see if the filename provided is the same as the filename of this directory
		if(strncmp((int8_t*)filename, (int8_t*)fname, NAME_SIZE) == 0 ){
			//if so, copy all relevant information into dentry
			curr_start = dir_entry_start + (i*dir_entry_length);
			memcpy(&(dentry->file_name), curr_start, NAME_SIZE);
 			memcpy(&(dentry->file_type), curr_start + file_name_length, SIZE_DATA_BLOCK);
 			memcpy(&(dentry->inode), curr_start + file_name_length + file_type_length, SIZE_DATA_BLOCK);
			return 0;
		}
		//otherwise, continue searching until we hit the actual entry
	}

	return -1;
}


/*
 * read_data
 *   DESCRIPTION: Given the inode, offset to begin at, and length, access the corresponding data and copy the data into buf 
 *   INPUTS: Inode, offset to begin at, buf to write data into, length of data to copy
 *   OUTPUTS: none
 *   RETURN VALUE: the number of bytes successfully copied, otherwise 0
 */ 
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){

	//check if inode is invalid
	if(inode >= boot_info.inodes || inode < 0){
		return -1; 
	}

	//calculate start address of the specific inode we are interested in, as well as the specific data block we would start getting data from 
	uint32_t* inode_start_addr = start_addr + BLOCK_SIZE + (inode * BLOCK_SIZE);
	uint32_t* data_block_start_addr = start_addr + BLOCK_SIZE + (boot_info.inodes * BLOCK_SIZE);

	uint32_t number_blocks, file_length;

	//copy the file length, from which we will calclate the maximum number of blocks we would potentially be interested in getting data from
	memcpy(&file_length, inode_start_addr, SIZE_DATA_BLOCK);

	if(offset >= file_length)
		return 0;
	number_blocks = file_length / BLOCK_SIZE_FOUR;

	//calculate individual (not full blocks of 4) and add that to total number of blocks
	if(file_length % BLOCK_SIZE_FOUR != 0)
		number_blocks++;

	uint32_t i = 0;

	//calculate full block (4) and individual blocks (<4) offset
	uint32_t offset_blocks = offset / BLOCK_SIZE_FOUR;
	uint32_t remaining_offset = offset % BLOCK_SIZE_FOUR;

	//offset to particular block we want
	uint32_t* starting_block = inode_start_addr + offset_blocks + 1;
	
	uint32_t block_number, *block_addr;
	uint32_t bytes_copied = 0;

	for(i = 0; i < number_blocks - offset_blocks; i++) {
		memcpy(&block_number, starting_block, SIZE_DATA_BLOCK);
		//get actual data from proper current block
		block_addr = data_block_start_addr + (block_number * BLOCK_SIZE);
		//if we have an offset
		if(remaining_offset != 0) {
			block_addr += remaining_offset/SIZE_DATA_BLOCK;
			if((BLOCK_SIZE_FOUR - remaining_offset) < length) {
				memcpy(buf+bytes_copied, block_addr, BLOCK_SIZE_FOUR - remaining_offset);
				//we copied some, so decrease length and increase the number of bytes we copied
				length -= (BLOCK_SIZE_FOUR - remaining_offset);
				bytes_copied += (BLOCK_SIZE_FOUR - remaining_offset);
			} else {
				memcpy(buf+bytes_copied, block_addr, length);
				bytes_copied += length;
				length = 0;
				break;
			}
			remaining_offset = 0;
		}else{ 
			//if no offset
			if(BLOCK_SIZE_FOUR  < length) {
				memcpy(buf+bytes_copied, block_addr, BLOCK_SIZE_FOUR);
				bytes_copied += BLOCK_SIZE_FOUR;
				length -= BLOCK_SIZE_FOUR; 
			} else {
				memcpy(buf+bytes_copied, block_addr, length);
				bytes_copied += length;
				length = 0;
				break;
			}
		}
		starting_block++;
	}

	//check to see if copying was done successfully
	if(length != 0)
		return 0;
	else
		return bytes_copied;
} 

/*start of system calls*/

/*
 * fs_open
 *   DESCRIPTION: opens a file
 *   INPUTS: filename
 *   OUTPUTS: none
 *   RETURN VALUE: 0
 */ 
int32_t fs_open(const uint8_t* filename){
	/*int index = DEFAULT_FD; 
	while(parent_pointer->FD[index].flags ==1 && index <FD_SIZE){
		index++;
	}

	dentry_t dentry_file_info;

	if(read_dentry_by_name((uint8_t*)filename, &dentry_file_info) == 0){
		uint32_t* inode_start_addr = start_addr + BLOCK_SIZE + (dentry_file_info.inode * BLOCK_SIZE);
		parent_pointer->FD[index].inode = inode_start_addr;
	}

	parent_pointer->FD[index].file_position = 0;
	parent_pointer->FD[index].flags = 1;*/
	return 0;
}

/*
 * fs_read
 *   DESCRIPTION: Read the file which is open
 *   INPUTS: int32_t fd, uint8_t* buf, int32_t nbytes
 *   OUTPUTS: none
 *   RETURN VALUE: bytes read on success, -1 on failure
 */ 
int32_t fs_read(int32_t fd, void* buf, int32_t nbytes) {
	dentry_t dentry;
	
	if(read_dentry_by_name((uint8_t*)buf, &dentry) == 0) {
		return read_data(dentry.inode, 0, (uint8_t*) buf, nbytes);
	} else {
		return -1;
	}
}

/*
 * fs_size
 *   DESCRIPTION: Get file length
 *   INPUTS: the file descriptor, a buf to read_dentry_byname, and the number of bytes to copy
 *   OUTPUTS: none
 *   RETURN VALUE: file length if success, otherwise -1
 */ 
int32_t fs_size(int32_t fd, void* buf, int32_t nbytes) {
	dentry_t dentry;

	if(read_dentry_by_name((uint8_t*)buf, &dentry) == 0) {
		uint32_t* inode_start_addr = start_addr + BLOCK_SIZE + (dentry.inode * BLOCK_SIZE);
		uint32_t file_length;
		memcpy(&file_length, inode_start_addr, SIZE_DATA_BLOCK);
		return file_length;
	} else {
		return -1;
	}
}

/*
 * fs_write  
 *   DESCRIPTION: Write the file
 *   INPUTS: int32_t fd, uint8_t* buf, int32_t nbytes
 *   OUTPUTS: none
 *   RETURN VALUE: bytes written on success, -1 on failure
 */ 
int32_t fs_write(int32_t fd, const void* buf, int32_t nbytes) {
	return -1;
}

/*
 * fs_close
 *   DESCRIPTION: Close the file
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success
 */
int32_t fs_close(int32_t fd) {
	return 0;
}

/*
 * dir_open
 *   DESCRIPTION: open the directory
 *   INPUTS: filename
 *   OUTPUTS: none
 *   RETURN VALUE: returns 0
 */ 
int32_t dir_open(const uint8_t* filename) {
	/*int index = DEFAULT_FD; 
	while(parent_pointer->FD[index].flags ==1 && index < FD_SIZE){
		index++;
	}

	parent_pointer->FD[index].inode = NULL;
	parent_pointer->FD[index].file_position = 0;
	parent_pointer->FD[index].flags = 1;*/
	
	return 0;
}
/*
 * dir_read
 *   DESCRIPTION: Read the directory which is open
 *   INPUTS: int32_t fd, uint8_t* buf, int32_t nbytes
 *   OUTPUTS: none
 *   RETURN VALUE: bytes read on success, 0 when all files are read
 */ 
int32_t dir_read(int32_t fd, void* buf, int32_t nbytes) {
	dentry_t dentry;

	if(read_dentry_by_index(dir_read_counter, &dentry) == 0) {
		memcpy(buf, dentry.file_name, NAME_SIZE);
		dir_read_counter++;
		return strlen((int8_t*)dentry.file_name);
	} else {
		dir_read_counter = 0;
		return 0;
	}
}

/*
 * dir_write
 *   DESCRIPTION: write directory
 *   INPUTS: int32_t fd, uint8_t* buf, int32_t nbytes
 *   OUTPUTS: none
 *   RETURN VALUE: 0
 */ 
int32_t dir_write(int32_t fd, const void* buf, int32_t nbytes) {
	return 0;
}

/*
 * dir_close
 *   DESCRIPTION: close directory
 *   INPUTS: int32_t fd
 *   OUTPUTS: none
 *   RETURN VALUE: 0
 */ 
int32_t dir_close(int32_t fd) {
	/*if(fd <DEFAULT_FD || fd >= FD_SIZE)
		return -1;

	parent_pointer->FD[fd].flags = 0;*/
	return 0;
}



/*
 * End of System Calls
 */ 

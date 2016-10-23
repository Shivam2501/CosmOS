#include "file.h"

uint32_t *start_addr, *dir_entry_start; 
uint32_t dir_read_counter = 0;

boot_t boot_info;

int get_file_system_start(unsigned int mod_start){
	start_addr = (uint32_t*) mod_start;

	dir_entry_start = (uint32_t*) (start_addr + 
				NUMBER_DIR_ENTRIES + NUMBER_INODES + NUMBER_DATA_BLOCKS + reserved0);

	memcpy(&(boot_info.dir_entries), start_addr, 4);
	memcpy(&(boot_info.inodes), start_addr + NUMBER_DIR_ENTRIES, 4);
	memcpy(&(boot_info.data_blocks), start_addr + NUMBER_DIR_ENTRIES + NUMBER_INODES, 4);

	uint8_t buf[40000];
	fs_read(1, buf, 40000);

	printf("%s\n", buf);

	return 0;
}

int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry){
	
	//check if index is invalid
	if(index >= boot_info.dir_entries){
		return -1; 
	}

	//memory starts at boot block, then block 0, gets specific block we need
 	uint32_t* curr_start = dir_entry_start + index*dir_entry_length;

 	memcpy(&(dentry->file_name), curr_start, 32);
 	memcpy(&(dentry->file_type), curr_start + file_name_length, 4);
 	memcpy(&(dentry->inode), curr_start + file_name_length + file_type_length, 4);

	return 0; 
}

int32_t read_dentry_by_name(const uint8_t* fname, dentry_t* dentry){

	if(strlen((int8_t*)fname) > 32)
		return -1; 
	
	uint32_t i;
	uint32_t* curr_start; 

	for(i = 0; i < boot_info.dir_entries; i++){
		uint8_t filename[32];
		memcpy(filename, dir_entry_start + (i*dir_entry_length), 32);

		if( strncmp ((int8_t*)filename, (int8_t*)fname, 32) == 0 ){
			curr_start = dir_entry_start + (i*dir_entry_length);
			memcpy(&(dentry->file_name), curr_start, 32);
 			memcpy(&(dentry->file_type), curr_start + file_name_length, 4);
 			memcpy(&(dentry->inode), curr_start + file_name_length + file_type_length, 4);
			return 0;
		}
	}

	return -1;
}

int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){

	//check if inode is invalid

	if(inode >= boot_info.inodes){
		return -1; 
	}

	uint32_t* inode_start_addr = start_addr + BLOCK_SIZE + (inode * BLOCK_SIZE);
	uint32_t* data_block_start_addr = inode_start_addr + (boot_info.inodes * BLOCK_SIZE);

	uint32_t number_blocks, file_length;

	memcpy(&file_length, inode_start_addr, 4);
	number_blocks = file_length / 4096;

	if(file_length % 4096 != 0)
		number_blocks++;

	uint32_t i = 0;

	uint32_t offset_blocks = offset / 4096;
	uint32_t remaining_offset = offset % 4096;

	uint32_t* starting_block = inode_start_addr + offset_blocks + 1;

	uint32_t block_number, *block_addr;
	uint32_t bytes_copied = 0;

	//printf("%d  %d %d\n", number_blocks, offset_blocks, remaining_offset );

	for(i = 0; i < number_blocks - offset_blocks; i++) {
		memcpy(&block_number, starting_block, 4);
		block_addr = data_block_start_addr + (block_number * BLOCK_SIZE);
		if(remaining_offset != 0) {
			block_addr += remaining_offset/4;
			if((4096 - remaining_offset) < length) {
				memcpy(buf+bytes_copied, block_addr, 4096 - remaining_offset);
				length -= (4096 - remaining_offset);
				bytes_copied += (4096 - remaining_offset);
			} else {
				memcpy(buf+bytes_copied, block_addr, length);
				bytes_copied += length;
				length = 0;
				break;
			}
			remaining_offset = 0;
			continue;
		}

		if(4096  < length) {
			memcpy(buf+bytes_copied, block_addr, 4096);
			bytes_copied += 4096;
			length -= 4096; 
		} else {
			memcpy(buf+bytes_copied, block_addr, length);
			printf("%s\n", buf);
			bytes_copied += length;
			length = 0;
			break;
		}

		starting_block++;
	}

	if(length != 0)
		return 0;
	else
		return bytes_copied;
}

/*
 * Start of System Calls
 */ 

/*
 * fs_open
 *   DESCRIPTION: Open the file
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: 0 on success
 */ 
int32_t fs_open(const uint8_t* filename) {
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

	uint8_t* name = (uint8_t *)"fish";
	if(read_dentry_by_name((uint8_t*)name, &dentry) == 0) {
		return read_data (dentry.inode, 0, (uint8_t*) buf, nbytes);
	} else {
		return -1;
	}
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
		memcpy(buf, dentry.file_name, 32);
		dir_read_counter++;
		return strlen((int8_t*)dentry.file_name);
	} else {
		dir_read_counter = 0;
		return 0;
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
 * End of System Calls
 */ 

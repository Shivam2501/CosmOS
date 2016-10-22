#include "file.h"

uint32_t *start_addr, *dir_entry_start; 

boot_t boot_info;

int get_file_system_start(unsigned int mod_start){
	start_addr = (uint32_t*) mod_start;

	dir_entry_start = (uint32_t*) (start_addr + 
				NUMBER_DIR_ENTRIES + NUMBER_INODES + NUMBER_DATA_BLOCKS + reserved0);

	memcpy(&(boot_info.dir_entries), start_addr, 4);
	memcpy(&(boot_info.inodes), start_addr + NUMBER_DIR_ENTRIES, 4);
	memcpy(&(boot_info.data_blocks), start_addr + NUMBER_DIR_ENTRIES + NUMBER_INODES, 4);

	/*printf("Root File Name: %d %d %d", boot_info.dir_entries, boot_info.inodes, boot_info.data_blocks);

	dentry_t* dentry;
	int i;
	for(i=0;i<boot_info.dir_entries;i++)
		read_dentry_by_index (i, dentry);*/

	/*dentry_t* dentry;
	uint8_t* name = "frame0.txt";
	read_dentry_by_name (name, dentry);
	printf("File: %s\n", dentry->file_name);
	printf("File: %d\n", dentry->file_type);
	printf("File: %d\n", dentry->inode);*/

	return 0;
}

int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry){
	
	printf("Index: %d", index);
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

	if(fname == NULL)
		return -1; 
	
	uint32_t i;
	uint32_t* curr_start; 

	for(i = 0; i < boot_info.dir_entries; i++){
		//file names match
		uint8_t filename[32];
		memcpy(filename, dir_entry_start + (i*dir_entry_length), 32);

		if( strncmp (filename, fname, 32) == 0 ){
			curr_start = dir_entry_start + (i*dir_entry_length);
			memcpy(&(dentry->file_name), curr_start, 32);
 			memcpy(&(dentry->file_type), curr_start + file_name_length, 4);
 			memcpy(&(dentry->inode), curr_start + file_name_length + file_type_length, 4);
			break;
		}
	}

	return 0;
}

/*
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){
	int i; 
	//check if invalid inode and return -1 
	//if bad data block found, return -1 

	uint32_t* curr_start;
	uint32_t* inode_start = dir_entry_start + (number_entries * dir_entry_length);
	uint32_t* = data_start = inode_start + (INODE_LENGTH * inodes_number);

	curr_start = inode_start + inode*INODE_LENGTH; 

	//go through the data to add to buffer
	uint32_t max_byte_to_copy_until = *((uint32_t*)start_addr);

	int off = 0, position = 0, blocknum, j = 0;
	uint32_t where_data_is; 

	for(i = 4; i < 4*(INODE_LENGTH); i++){
  		if(off < offset){
  			off++; 
  			continue; 
  		}

		//we took correct number of bytes
  		if(off == length || off == max_byte_to_copy_until)
  			break; 

  		blocknum = off / 4; 

  		where_data_is = *((uint32_t*) (inode_start + 4(blocknum + 1))); // magic number define later!!!

  		buf[j] = *((uint8_t*) (data_start + where_data_is*INODE_LENGTH + position))

  		j++;
  		position++;
  		off++;
  		if(position > 3)
  			position = 0;

	}


	return 0; 
}



int32_t open (const uint8 t* filename){
	return -1; 
}

int32_t close (int32_t fd){
	return -1; 
}
*/
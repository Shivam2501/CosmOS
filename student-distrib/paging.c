/*
// typedef union page_directory_t {
// 	uint32_t val[2];
// 	struct {
// 		uint16_t present: 1;
// 		uint16_t read_write: 1;
// 		uint8_t user_supervisor: 1;
// 		uint32_t write_through : 1;
// 		uint32_t cache_disabled : 1;
// 		uint32_t accessed : 1;
// 		uint32_t reserved0 : 1;
// 		uint32_t page_size : 1;
// 		uint32_t ignored : 1;
// 		uint32_t available : 3;
// 		uint32_t page_table_addr : 20;
// 	} __attribute__((aligned(4096)));
// } page_directory_t;

// typedef union page_table_t {
// 	uint32_t val[2];
// 	struct {
// 		uint16_t present : 1;
// 		uint16_t read_write : 1;
// 		uint8_t user_supervisor : 1;
// 		uint32_t write_through : 1;
// 		uint32_t cache_disabled : 1;
// 		uint32_t accessed : 1;
// 		uint32_t dirty : 1;
// 		uint32_t reserved0 : 1;
// 		uint32_t global : 1;
// 		uint32_t available: 3;
// 		uint16_t physical_page_addr : 20;
// 	} __attribute__((aligned(4096)));
// } page_table_t;

//enable paging
asm volatile("                      	\n\
			push    %%ebp           	\n\
			mov     %%esp, %%ebp    	\n\
			mov     8(%%esp), %%eax     \n\
			mov     %%eax, %%cr3     	\n\
			mov 	%%ebp, %%esp 		\n\
			pop		%%ebp				\n\
			ret 
			"
			:
			:
			);


uint32_t page_directory[1024] __attribute__((aligned(4096)));
uint32_t page_table[1024] __attribute__((aligned(4096)));

//set all present to 0
int i;
for(int i =0; i < 1024; i++){
	page_directory[i] = = 0x00000002;
	page_table[i] = = 0x00000002;
}

uint16_t index_to_directory, index_to_table;
index_to_directory = ((VIDEO & 0xFFC00000) >> 22); //gets top ten most significant bits for directory index 
index_to_table = ((VIDEO & 0x3FF000) >> 12);  //gets next ten most significant bits for table index 

page_directory[index_to_directory] = page_table | 0x3; //set read/write to 1 and present to 1
page_table[index_to_table] = VIDEO | 0x3; 

*/

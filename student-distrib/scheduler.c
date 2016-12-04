#include "scheduler.h"

//current task running
int current_task = 0;


int next_task() {
	int prev_task = current_task;
	//calculate the next task
	current_task = (current_task + 1) % NUMBER_TERMINALS;

	//find an active task
	int counter = 0;
	while(terminals[current_task].current_process == NULL && counter < 3) {
		current_task ++;
		current_task %= NUMBER_TERMINALS;
		counter++;
	}
	if(prev_task == current_task || counter == 3) {
		return -1;
	}
	return 0;
}

void context_switch() {

	//inline assembly - save current context
	asm volatile("                  \n\
			movl    %%esp, %0   	\n\
			movl 	%%ebp, %1	    \n\
			pushfl					\n\
			popl	%2				\n\
			"
			: "=S"(terminals[current_task].esp), "=b"(terminals[current_task].ebp), "=c"(terminals[current_task].eflags)
			:
			: "memory", "cc"
			);

	
	if(next_task() == -1)
		return;

	//paging to new current process
	add_paging(PAGE_DIR_ENTRY, (KERNEL_PROCESS_START + (terminals[current_task].current_process->pid)*KERNEL_PROCESS_SIZE));

	//tss.ss0 = KERNEL_DS;
	tss.esp0 = KERNEL_PROCESS_START - (terminals[current_task].current_process->pid)*KERNEL_STACK_SIZE - PAGE_ALIGNMENT;

	//switch to new process
	asm volatile("                  \n\
				movl    %0, %%esp   	\n\
				movl 	%1, %%ebp 	    \n\
				push    %2				\n\
				popfl					\n\
				"
				:
				: "r"(terminals[current_task].esp), "r"(terminals[current_task].ebp), "r"(terminals[current_task].eflags)
				: "memory", "cc"
				);

}

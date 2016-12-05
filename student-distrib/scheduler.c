#include "scheduler.h"

//current task running
volatile int current_task = 0;
int prev_task = 0;

/*
 * next_task
 *   DESCRIPTION: Next task to schedule
 *   INPUTS: none
 *   OUTPUTS: new task which will be executed
 *   RETURN VALUE: 0 on success, -1 when no task available
 */ 
int next_task() {
	prev_task = current_task;
	//calculate the next task
	current_task = (current_task + 1) % NUMBER_TERMINALS;

	//find an active task
	int counter = 0;
	while(terminals[current_task].current_process == NULL && counter < NUMBER_TERMINALS) {
		current_task ++;
		current_task %= NUMBER_TERMINALS;
		counter++;
	}
	if(prev_task == current_task || counter == NUMBER_TERMINALS) {
		return -1;
	}
	return 0;
}

/*
 * context_switch
 *   DESCRIPTION: Switch between terminals
 *   INPUTS: none
 *   OUTPUTS: schedules the new task to run
 *   RETURN VALUE: none
 */ 
void context_switch() {

	if(next_task() == -1)
		return;

	//inline assembly - save current context
	asm volatile("                  \n\
			movl    %%esp, %0   	\n\
			movl 	%%ebp, %1	    \n\
			pushfl					\n\
			popl	%2				\n\
			"
			: "=S"(terminals[prev_task].esp), "=b"(terminals[prev_task].ebp), "=c"(terminals[prev_task].eflags)
			:
			: "memory", "cc"
			);

	//paging to new current process
	add_paging(PAGE_DIR_ENTRY, (KERNEL_PROCESS_START + (terminals[current_task].current_process->pid)*KERNEL_PROCESS_SIZE));

	//tss.ss0 = KERNEL_DS;
	tss.esp0 = KERNEL_PROCESS_START - (terminals[current_task].current_process->pid)*KERNEL_STACK_SIZE - PAGE_ALIGNMENT;

	//switch to new process
	asm volatile("            		    \n\
				movl    %0, %%esp   	\n\
				movl 	%1, %%ebp 	    \n\
				pushl    %2				\n\
				popfl					\n\
				"
				:
				: "r"(terminals[current_task].esp), "r"(terminals[current_task].ebp), "r"(terminals[current_task].eflags)
				: "memory", "cc"
				);
}

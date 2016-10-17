#define NUMBER_HANDLER    48
#define IDT_TOTAL_NUMBER  256
#define	SYSTEM_CALL_ADDR  128

#include "idt_handlers.h"
#include "lib.h"
#include "rtc.h"
#include "i8259.h"
#include "asm_linkage.h"


/* 
 * init_idt
 *   DESCRIPTION: Creates entries in the IDT for interrupts, exceptions, and system calls 
 *   INPUTS: none
 *   OUTPUTS: none
 *   RETURN VALUE: none
 *   SIDE EFFECTS: 
 */
void init_idt(){ 
	/*Calls a macro for all the exceptions, interrupts and system calls, passing the idt array and specific function handler*/
	SET_IDT_ENTRY(idt[0],&DIVIDE_BY_ZERO);
	SET_IDT_ENTRY(idt[1],&DEBUG_EXCEPTION);
	SET_IDT_ENTRY(idt[2],&NMI_INTERRUPT);
	SET_IDT_ENTRY(idt[3],&BREAKPOINT_EXCEPTION);
	SET_IDT_ENTRY(idt[4],&OVERFLOW_EXCEPTION);
	SET_IDT_ENTRY(idt[5],&BOUND_RANGE_EXCEEDED_EXCEPTION);
	SET_IDT_ENTRY(idt[6],&INVALID_OPCODE_EXCEPTION);
	SET_IDT_ENTRY(idt[7],&DEVICE_NOT_AVAILABLE_EXCEPTION);
	SET_IDT_ENTRY(idt[8],&DOUBLE_FAULT_EXCEPTION);
	SET_IDT_ENTRY(idt[9],&COPROCESSOR_SEGMENT_OVERRUN);
	SET_IDT_ENTRY(idt[10],&INVALID_TSS_EXCEPTION);
	SET_IDT_ENTRY(idt[11],&SEGMENT_NOT_PRESENT_EXCEPTION);
	SET_IDT_ENTRY(idt[12],&STACK_FAULT_EXCEPTION);
	SET_IDT_ENTRY(idt[13],&GENERAL_PROTECTION_EXCEPTION);
	SET_IDT_ENTRY(idt[14],&PAGE_FAULT_EXCEPTION);
	SET_IDT_ENTRY(idt[16],&x86_FPU_FLOATING_POINT_ERROR);
	SET_IDT_ENTRY(idt[17],&ALIGNMENT_CHECK_EXCEPTION); 
	SET_IDT_ENTRY(idt[18],&MACHINE_CHECK_EXCEPTION); 
	SET_IDT_ENTRY(idt[19],&SIMD_FLOATING_POINT_EXCEPTION); 
	SET_IDT_ENTRY(idt[20],&INTEL_RESERVED); 
	SET_IDT_ENTRY(idt[21],&INTEL_RESERVED);
	SET_IDT_ENTRY(idt[22],&INTEL_RESERVED);
	SET_IDT_ENTRY(idt[23],&INTEL_RESERVED);
	SET_IDT_ENTRY(idt[24],&INTEL_RESERVED);
	SET_IDT_ENTRY(idt[25],&INTEL_RESERVED);
	SET_IDT_ENTRY(idt[26],&INTEL_RESERVED);
	SET_IDT_ENTRY(idt[27],&INTEL_RESERVED);
	SET_IDT_ENTRY(idt[28],&INTEL_RESERVED);
	SET_IDT_ENTRY(idt[29],&INTEL_RESERVED);
	SET_IDT_ENTRY(idt[30],&INTEL_RESERVED);
	SET_IDT_ENTRY(idt[31],&INTEL_RESERVED);
	SET_IDT_ENTRY(idt[32],&asm_timer_chip);
	SET_IDT_ENTRY(idt[33],&asm_keyboard);
	SET_IDT_ENTRY(idt[34],&asm_default); 
	SET_IDT_ENTRY(idt[35],&asm_default); 
	SET_IDT_ENTRY(idt[36],&asm_default); 
	SET_IDT_ENTRY(idt[37],&asm_default); 
	SET_IDT_ENTRY(idt[38],&asm_default); 
	SET_IDT_ENTRY(idt[39],&asm_default); 
	SET_IDT_ENTRY(idt[40],&asm_rtc);
	SET_IDT_ENTRY(idt[41],&asm_default); 
	SET_IDT_ENTRY(idt[42],&asm_default); 
	SET_IDT_ENTRY(idt[43],&asm_default); 
	SET_IDT_ENTRY(idt[44],&asm_default); 
	SET_IDT_ENTRY(idt[45],&asm_default); 
	SET_IDT_ENTRY(idt[46],&asm_default); 
	SET_IDT_ENTRY(idt[SYSTEM_CALL_ADDR],&SYSTEM_CALL);
}


/* 
 *   Exception handlers
 *   DESCRIPTION: handlers for all exceptions, including specific ones and intel reserved exceptions
 *   INPUTS: none
 *   OUTPUTS: prints warning
 *   RETURN VALUE: none
 *   SIDE EFFECTS: Remains in a while loop as it is an exception
 */
void DIVIDE_BY_ZERO(){
	printf("Warning: Divided by zero");
	while(1); 
}

void DEBUG_EXCEPTION(){
	printf("Warning: Debug exception");
	while(1);
}

void NMI_INTERRUPT(){
	printf("Warning: NMI Interrupt");
	while(1);
}

void BREAKPOINT_EXCEPTION(){
	printf("Warning: Breakpoint Exception");
	while(1);
}

void OVERFLOW_EXCEPTION(){
	printf("Warning: Overflow Exception");
	while(1);
}
void BOUND_RANGE_EXCEEDED_EXCEPTION(){
	printf("Warning: Bound Range Exceeded Exception");
	while(1);
}
void INVALID_OPCODE_EXCEPTION(){
	printf("Warning: Invalid Opcode Exception");
	while(1);
}
void DEVICE_NOT_AVAILABLE_EXCEPTION(){
	printf("Warning: Device Not Available Exception");
	while(1);
}
void DOUBLE_FAULT_EXCEPTION(){
	printf("Warning: Double Fault Exception");
	while(1);
}
void COPROCESSOR_SEGMENT_OVERRUN(){
	printf("Warning: Coprocessor Segment Overrun");
	while(1);
}
void INVALID_TSS_EXCEPTION(){
	printf("Warning: Invalid TSS Exception");
	while(1);
}
void SEGMENT_NOT_PRESENT_EXCEPTION(){
	printf("Warning: Segment Not Present Exception");
	while(1);
}
void STACK_FAULT_EXCEPTION(){
	printf("Warning: Stack Fault Exception");
	while(1);
}
void GENERAL_PROTECTION_EXCEPTION(){
	printf("Warning: General Protection Exception");
	while(1);
}
void PAGE_FAULT_EXCEPTION(){
	printf("Warning: Page Fault Exception");
	while(1);
}
void x86_FPU_FLOATING_POINT_ERROR(){
	printf("Warning: FPU Floating Point Error");
	while(1);
}

void ALIGNMENT_CHECK_EXCEPTION(){
	printf("Warning: Alignment Check Exception");
	while(1);
}
void MACHINE_CHECK_EXCEPTION(){
	printf("Warning: Machine Check Exception");
	while(1);
}
void SIMD_FLOATING_POINT_EXCEPTION(){
	printf("Warning: SIMD Floating Point Exception");
	while(1);
}
void INTEL_RESERVED(){
	printf("Warning: Intel Reserved");
	while(1);
}

/*Interrupt Handlers*/

/* 
 *   Interrupt default
 *   DESCRIPTION: handlers for most interrupts, all the general ones call this default handler
 *   INPUTS: none
 *   OUTPUTS: prints notification
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
void INTERRUPT_DEFAULT(){
	printf("Standard Default for other interrupts");
}

/* 
 *   handler for PIT (Timer Chip)
 *   DESCRIPTION: Is the handler called for PIT. It sends an end-of-interrupt signal.
 *   INPUTS: none
 *   OUTPUTS: prints warning
 *   RETURN VALUE: none
 *   SIDE EFFECTS: 
 */
void TIMER_CHIP(){
	// printf("Timer Chip Handled\n");
	outb(0x20, 0x20);
	sti();
}


/* 
 *   Handler for System Calls
 *   DESCRIPTION: Is the handler called for all system calls. 
 *   INPUTS: none
 *   OUTPUTS: prints notification
 *   RETURN VALUE: none
 *   SIDE EFFECTS: 
 */
void SYSTEM_CALL(){
	printf("Call for any system call");
}

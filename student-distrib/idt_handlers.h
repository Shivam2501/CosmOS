#ifndef _IDT_HANDLERS
#define _IDT_HANDLERS

#include "x86_desc.h"
#include "lib.h"
#include "rtc.h"
#include "i8259.h"
#include "asm_linkage.h"
#include "idt_system_call.h"

#define NUMBER_HANDLER    	48
#define IDT_TOTAL_NUMBER  	256
#define	SYSTEM_CALL_ADDR  	128
#define	EOI_SIGNAL 			0x20



void init_idt();

/*Exceptions*/
void DIVIDE_BY_ZERO();

void DEBUG_EXCEPTION();

void NMI_INTERRUPT();

void BREAKPOINT_EXCEPTION();

void OVERFLOW_EXCEPTION();

void BOUND_RANGE_EXCEEDED_EXCEPTION();

void INVALID_OPCODE_EXCEPTION();

void DEVICE_NOT_AVAILABLE_EXCEPTION();

void DOUBLE_FAULT_EXCEPTION();

void COPROCESSOR_SEGMENT_OVERRUN();

void INVALID_TSS_EXCEPTION();

void SEGMENT_NOT_PRESENT_EXCEPTION();

void STACK_FAULT_EXCEPTION();

void GENERAL_PROTECTION_EXCEPTION();

void PAGE_FAULT_EXCEPTION();

void x86_FPU_FLOATING_POINT_ERROR();

void ALIGNMENT_CHECK_EXCEPTION();

void MACHINE_CHECK_EXCEPTION();

void SIMD_FLOATING_POINT_EXCEPTION();

void INTEL_RESERVED();

/*Interrupt Handlers*/
void RTC_HANDLER();

void KEYBOARD_HANDLER();

void INTERRUPT_DEFAULT();

/*System Calls*/
void SYSTEM_CALL();


#endif /* _IDT_HANDLERS */

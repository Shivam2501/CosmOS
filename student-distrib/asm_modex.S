#define ASM     1

#switch to mode 13h
.globl graphics_mode
graphics_mode:
xorl %eax, %eax
movw $0x13, %ax
int $0x10

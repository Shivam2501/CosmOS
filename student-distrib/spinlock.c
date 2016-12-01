#include "spinlock.h"

void spin_lock_init(spinlock_t *lock) {
	lock->slock = SPINLOCK_UNLOCKED;
}

//acquire a spin lock, spin waiting if it is not available
void spin_lock(spinlock_t *lock) {
	asm volatile (" 				\n\
			acquire_lock: 			\n\
			movl $1, %%eax; 		\n\
			xchgl %%eax,(%%ebx); 	\n\
			test %%eax, %%eax; 		\n\
			jnz acquire_lock; 		\n\
			"
			:
			: "b" (&lock->slock)
			: "eax", "cc"
			);
}

void spin_lock_irqsave(spinlock_t *lock, unsigned long flags) {
	//block interrupts
	asm volatile (" 		\n\
			pushfl; 		\n\
			popl %0; 		\n\
			cli; 			\n\
			"
			:
			: "r"(flags)
			: "cc"
			);
	spin_lock(lock);
}

//release a spin lock
void spin_unlock(spinlock_t *lock) {
	lock->slock = SPINLOCK_UNLOCKED;
}

void spin_unlock_irqrestore(spinlock_t *lock, unsigned long flags) {
	spin_unlock(lock);
	//restore interrupts
	asm volatile (" 	\n\
			pushl %0; 	\n\
			popfl;	 	\n\
			"
			:
			: "r" (flags)
			: "cc"
			);
}

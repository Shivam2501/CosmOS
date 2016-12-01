#include "spinlock.h"

void spin_lock_init(spinlock_t *lock) {
	lock->slock = SPINLOCK_UNLOCKED;
}

//acquire a spin lock, spin waiting if it is not available
void spin_lock(spinlock_t *lock) {
	asm volatile (" 				\n\
			movl $1, %%eax; 		\n\
			acquire_lock: 			\n\
			xchgl %%eax,(%0);	 	\n\
			cmpl %1, %%eax; 		\n\
			je acquire_lock; 		\n\
			"
			:
			: "r" (&(lock->slock)), "r" (SPINLOCK_LOCKED)
			: "eax", "cc", "memory"
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
			: "cc", "memory"
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

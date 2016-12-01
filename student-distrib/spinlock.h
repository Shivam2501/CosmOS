#ifndef SPINLOCK_H
#define SPINLOCK_H

#include "types.h"
#include "i8259.h"
#include "lib.h"

#define SPINLOCK_LOCKED		1
#define SPINLOCK_UNLOCKED	0

typedef struct spinlock {
	volatile unsigned int slock;
} spinlock_t;

void spin_lock_init(spinlock_t *lock);

void spin_lock(spinlock_t *lock);
void spin_lock_irqsave(spinlock_t *lock, unsigned long flags);

void spin_unlock(spinlock_t *lock);
void spin_unlock_irqrestore(spinlock_t *lock, unsigned long flags);

#endif

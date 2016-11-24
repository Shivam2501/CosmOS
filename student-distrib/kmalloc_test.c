#include "kmalloc_test.h"

void test_kmalloc_1(){
	int *p = kmalloc(sizeof(int));
	*p = 5;

	kfree(p);
	
	int *p2 = kmalloc(sizeof(int));
	*p2 = 670;

	printf("%d %d\n", *p, *p2);
	kfree(p2);
}

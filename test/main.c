#include "pos-lib.h"
#include <stdio.h>

int main(){
	void *pa, *pb;

printf("!\n");

	pos_create("A");
	pos_create("B");

printf("!!\n");

	pa = pos_malloc("A", 1024);
	pb = pos_malloc("B", 1024);

printf("!!!\n");

	printf("pa = %p \n", pa);
	printf("pb = %p \n", pb);

	pos_free("A", pa);
	pos_free("B", pb);

	return 0;
}

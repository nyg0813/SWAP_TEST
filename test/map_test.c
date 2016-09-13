#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

int main(int argc, char* argv[])
{
	unsigned long long sum=0;
	int i;
	pos_create(argv[0]);
	pos_unmap(argv[0]);
	for(i=0;i<3;i++)
	{
		struct timespec start,end,zero;
		unsigned long long diff;
		unsigned long long diff_b;
		unsigned long long diff_a;
		pos_unmap(argv[0]);
		clock_gettime(CLOCK_REALTIME, &start);
		diff_b = (start.tv_nsec)/1000000;
		printf("before : %llu\n",diff_b);
		pos_map(argv[0]);
		clock_gettime(CLOCK_REALTIME, &end);
		diff_a = (end.tv_nsec)/1000000;
		printf("after : %llu\n",diff_a);
		diff = (diff_a-diff_b);
		sum += diff;
		printf("diff : %llu\n",diff);
	}

	printf("sum : %llu, avg : %llu\n",sum ,sum/3);
	pos_delete(argv[0]);
	return 0;
}

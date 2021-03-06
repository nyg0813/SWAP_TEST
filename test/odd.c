#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include "pos-lib.h"


int pos_num;
int opt;
int extra;

long long get_current_utime(void)
{
        struct timeval current;

        gettimeofday(&current,NULL);

        return (current.tv_sec*1000000 + current.tv_usec);
}

void create_base_object_stores(void)
{
	int i;
	char buf[5];
	for(i=0;i<pos_num;i++)
	{
		sprintf(buf,"%d",i);
		pos_create(buf);
	}
}
void create_next_object_stores(void)
{
	int i;
	char buf[5];
	for(i=pos_num;i<pos_num+extra;i++)
	{
		sprintf(buf,"%d",i);
		pos_create(buf);
	}
}
void unmap_some_objects(int per)
{
	int i=0;
	char buf[5];
	for(i=0;i<pos_num;i++)
	{

		if(i>200&&i<=300)
		{
			sprintf(buf,"%d",i);
			pos_unmap(buf);
		}
	}
}
void unmap_all_objects(void)
{
	int i=0;
	char buf[5];
	for(i=0;i<pos_num;i++)
	{
		sprintf(buf,"%d",i);
		pos_unmap(buf);
	}
}
void delete_all_objects(void)
{
	int i=0;
	char buf[5];
	for(i=0;i<pos_num+extra;i++)
	{
		sprintf(buf,"%d",i);
		pos_map(buf);
		pos_delete(buf);
	}
}
void map_all_objects(void)
{
	int i=0;
	char buf[5];
	for(i=0;i<pos_num+extra;i++)
	{
		sprintf(buf,"%d",i);
		pos_map(buf);
	}
}
void fper(void)
{
	int fd;	
	int i;
	char buf[5];
	char* alpha = (char*)malloc(1024*1024);
	for(i=0;i<pos_num;i++)
	{
		if((i>100&&i<=200)||(i>300&&i<=400))
		{
			char p[20];
			char* pos_buf;
			sprintf(buf,"%d",i);
			pos_map(buf);
			printf("%d pos is mapped and buf is %s\n",i,buf);
			pos_buf = (char*)pos_get_prime_object(buf);
			sprintf(p,"%p",pos_buf);
			printf("%d pos prime is %s\n",i,p);
			memcpy(alpha,pos_buf,1024*1024);
		}
	}
	free(alpha);
}
void tper(void)
{
	int fd;	
	int i=0;
	char buf[5];
	char* alpha = (char*)malloc(1024*1024);
	for(i=0;i<pos_num;i++)
	{
		if(i>300&&i<=400)
		{
			char p[20];
			char* pos_buf;
			sprintf(buf,"%d",i);
			pos_map(buf);
			printf("%d pos is mapped and buf is %s\n",i,buf);
			pos_buf = (char*)pos_get_prime_object(buf);
			sprintf(p,"%p",pos_buf);
			printf("%d pos prime is %s\n",i,p);
			memcpy(alpha,pos_buf,1024*1024);
		}
	}
	free(alpha);
}

void alloc_base_pos(void)
{
	int fd = open("first.txt", O_RDWR|O_TRUNC|O_CREAT,0644);
	int i=0;
	char buf[5];
	char p[50];
	char p2[50];
	char* alpha = (char*)malloc(1024*1024);
        memset(alpha,'f', 1024*1024);
	int index=0;
	void* pos_buf;
	void* temp;
	for(i=0;i<pos_num;i++)
	{
		sprintf(buf,"%d",i);
		pos_buf = pos_malloc(buf,1024*1024);
		memcpy(pos_buf,alpha,1024*1024);
		sprintf(p,"%p\n",(char*)pos_buf);
		write(fd,pos_buf,1024*1024);
		//write(fd,p,strlen(p));
		pos_set_prime_object(buf,pos_buf);
		//temp = pos_get_prime_object(buf);
		//printf("%s's pos_buf %s %p\n",buf,p,temp);
	}
}

void alloc_next_pos(void)
{
	int fd = open("forth.txt", O_RDWR|O_TRUNC|O_CREAT,0644);
	int i=0;
	char buf[5];
	char p[50];
	char* alpha = (char*)malloc(1024*1024);
        memset(alpha,'f', 1024*1024);
	int index=0;
	char* pos_buf;
	for(i=pos_num;i<pos_num+extra;i++)
	{
		sprintf(buf,"%d",i);
		pos_buf = (char*)pos_malloc(buf,1024*1024);
		pos_set_prime_object(buf,(void*)pos_buf);
		memcpy(pos_buf,alpha,1024*1024);
		sprintf(p,"%p\n",pos_buf);
		write(fd,pos_buf,1024*1024);
		//write(fd,p,strlen(p));
		//printf("pos_buf : %s",p);
	}
}
int main(int argc, char* argv[])
{
	pos_num		= atoi(argv[1]);
	opt 		= atoi(argv[2]);
	extra 		= atoi(argv[3]);
	long long io_time_start, io_time_end;
	if(opt == 1)
	{
		int r,t,f;
		create_base_object_stores();
		printf("Create complete\n");
		alloc_base_pos();
		printf("Alloc complete\nWant unmap?\n");
		scanf("%d",&t);
		unmap_some_objects(50);
		printf("Unmapped completed\n");
		scanf("%d",&r);
		create_next_object_stores();
		printf("Next create complete\n");
		io_time_start = get_current_utime();
		alloc_next_pos();
		io_time_end = get_current_utime();
		printf("Next alloc complete\n");
		printf("[POS Test] Test end. Execution time: %lld usec\n",(io_time_end - io_time_start));
		scanf("%d",&f);
		delete_all_objects();
		printf("Unmapping all Objects\n");
	}
	else if(opt == 2)
	{
		int t;
		fper();
		printf("50 per Create and Alloc complete\n");
		scanf("%d",&t);
	}
	else if(opt == 3)
	{
		int t;
		tper();
		printf("25 per Create and Alloc complete\n");
		scanf("%d",&t);
	}

}



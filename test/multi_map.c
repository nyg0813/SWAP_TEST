#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include "pos-lib.h"


int main(int argc, char* argv[])
{
	char* object_name;
	int process_num, opt;
	object_name	= argv[1];
	process_num	= atoi(argv[2]);
	opt 		= atoi(argv[3]);
	char alpha[1024*4];
	char beta[1024*4];
	int index=0;
	memset(alpha,'f',1024*4);
	memset(beta,'b',1024*4);
	char* pos_buf=NULL;
	if(opt)
	{
		int f;
		printf("Maker make %s\n",object_name);
		pos_create(object_name);
		printf("Maker before pos_malloc %p\n",pos_buf);
		pos_buf = (char*)pos_malloc(object_name,1024*4);
		printf("Maker after pos_malloc %p\n",pos_buf);
		printf("Maker memcpy!\n");			
		memcpy(pos_buf,alpha,1024*4);
		printf("Maker memcpy success!\n");
		//pos_free(object_name,pos_buf);
		//printf("Maker free end!\n");
		printf("I'm Maker %d, %s\n",getpid(),pos_buf);
		scanf("%d",&f);
		printf("Maker after Mapper change %d, %s\n",getpid(),pos_buf);
		pos_delete(object_name);
	}
	else
	{
		int f,tmep;
		printf("I'm mapper %d\n",getpid());
		printf("Mapper map %s\n",object_name);
		pos_map(object_name);
		printf("Mapper map %s success\n",object_name);
		scanf("%d",&f);
		printf("Mapper before pos_malloc %p\n",pos_buf);
		pos_buf = (char*)0x7c001008;
		printf("Mapper after pos_malloc %p\n",pos_buf);
		printf("Mapper memcpy!\n");			
		memcpy(pos_buf,beta,1024*4);
		printf("Mapper memcpy success!%s\n",pos_buf);
		scanf("%d",&tmep);
		pos_unmap(object_name);
		printf("Unmap success!\n");
	}
			

	/*
	int i;
	for(i=0;i<process_num;i++)
	{
		pid[i] = fork();
		if(pid[i]<0)
		{
			printf("fork fail\n");			
		}
		else if(pid[i] == 0)
		{
			sleep(10000);
			printf("child map %s\n",object_name);
			//pos_map(object_name);
			printf("access success\n");
			if(pos_buf)
			{
				printf("child success\n");
				printf("c memcpy!\n");			
				memcpy(pos_buf,"1234",1024*4);
				printf("c memcpy success!\n");	
			}
			else
				printf("child fail\n");	
		}
		else
		{
			
			printf("I'm parents %d\n",pa);
			scanf("%d",&f);
			pos_delete(object_name);			
		}
	}
	*/
}

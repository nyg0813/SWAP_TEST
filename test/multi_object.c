#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "pos-lib.h"

#define N_OBJECT_NAME	10

int n_objects;
int object_size_1k;
int object_size;
char** arr_object_name;
char** p_object;

void init_test_object_names(void);
void create_test_objects(void);
void open_test_objects(void);
void alloc_memory_to_test_objects(void);
void unmap_test_objects(void);
void free_test_objects(void);
void delete_test_objects(void);

int main(int argc, char* argv[])
{
	int i;
	int temp;

	n_objects		= atoi(argv[1]);
	object_size_1k 		= atoi(argv[2]);	// total object size (kbyte)
	object_size 		= object_size_1k * 1024;	// object size for each threads

	printf("[POS TEST] Start Test for %d objects (%d Kbyte)\n", n_objects, object_size_1k);

	init_test_object_names();
	printf("\t--Init object names complete!\n");

	create_test_objects();
	printf("\t--Create objects complete! \n");

	alloc_memory_to_test_objects();
	printf("\t--Alloc memory complete! \n");

//	scanf("%d",&temp);
//	unmap_test_objects();
//	printf("\t--Unmap test objects complete! \n");

	//free_test_objects();
	//printf("\t--Free test objects complete! \n");

	delete_test_objects();
	printf("\t--Delete test objects complete! \n");

	/* Free memory for name array */
        for(i=0; i<n_objects; i++){
                free(arr_object_name[i]);
        }
	free(arr_object_name);

	printf("[POS Test] Test end. \n");
	return 0;
}

void init_test_object_names(void)
{
	int i;
	char object_name[N_OBJECT_NAME];

	/* Allocate Memory for object name variable */
	arr_object_name = (char**)malloc(sizeof(object_name) * n_objects);
	for(i=0; i<n_objects; i++){
		arr_object_name[i] = (char*)malloc(sizeof(object_name));
	}

	p_object = (char**)malloc(sizeof(char*) * n_objects);
	for(i=0; i<n_objects; i++){
		p_object[i] = (char*)malloc(sizeof(char*));
	}

	for(i=0; i<n_objects; i++){
		/* Reset object name */
		memset(object_name, '\0', sizeof(object_name));

		/* Make object name*/
		sprintf(object_name,"%d", i+1);
	
		/* Update object name array */
		strcpy(arr_object_name[i], object_name);
	}
}

void create_test_objects(void)
{
	int i;
	for(i = 0; i<n_objects; i++){
		pos_create(arr_object_name[i]);
	}
}  

void alloc_memory_to_test_objects(void)
{
	int i;
	char* buf = (char*)malloc(object_size);
	memset(buf, 0xcafe, object_size);

	for(i = 0; i<n_objects; i++){
		p_object[i] = (char*)pos_malloc(arr_object_name[i], object_size);
		memcpy(p_object[i], buf, object_size);
	}

	free(buf);
}

void unmap_test_objects(void)
{
	int i;
	for(i = 0; i<n_objects; i++){
		pos_unmap(arr_object_name[i]);
	}
}

void free_test_objects(void)
{
	int i;
	for(i = 0; i<n_objects; i++){
		pos_free(arr_object_name[i], p_object[i]);
	}
}

void delete_test_objects(void)
{
	int i;
	for(i = 0; i<n_objects; i++){
		pos_delete(arr_object_name[i]);
	}
}

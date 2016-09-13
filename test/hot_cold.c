#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define POS
#ifdef POS
	#include "pos-lib.h"
#endif

#define N_OBJECT_NAME	10
#define HOT_ACCESS_RATIO	80

int n_objects; 		// # of total objects
int n_hot_objects; 	// # of hot objects
int n_cold_objects; 	// # of cold objects
int n_updates; 		// # of updates

int object_size_1k;
int object_size;
int hot_ratio;
char** arr_object_name;
char** arr_hot_object;
char** arr_cold_object;
char** p_hot_object;
char** p_cold_object;

void init_test_object_names(void);
void create_test_objects(void);
void open_test_objects(void);
void alloc_memory_to_test_objects(void);
long long update_test_objects(void);
void delete_test_objects(void);

#define NN 312
#define MM 156
#define MATRIX_A 0xB5026F5AA96619E9ULL
#define UM 0xFFFFFFFF80000000ULL /* Most significant 33 bits */
#define LM 0x7FFFFFFFULL /* Least significant 31 bits */

/* The array for the state vector */
static unsigned long long mt[NN];

/* mti==NN+1 means mt[NN] is not initialized */
static int mti=NN+1;

void init_genrand64(unsigned long long seed)
{
        mt[0] = seed;
        for (mti=1; mti<NN; mti++)
                mt[mti] =  (6364136223846793005ULL * (mt[mti-1] ^ (mt[mti-1] >> 62)) + mti);
}

unsigned long long genrand64_int64(void)
{
        int i;
        unsigned long long x;
        static unsigned long long mag01[2]={0ULL, MATRIX_A};

        if (mti >= NN) { /* generate NN words at one time */

                /* if init_genrand64() has not been called, */
                /* a default initial seed is used     */
                if (mti == NN+1){
//                      srand((unsigned)time(NULL));
                        init_genrand64((unsigned long long)rand());
                }

                for (i=0;i<NN-MM;i++) {
                        x = (mt[i]&UM)|(mt[i+1]&LM);
                        mt[i] = mt[i+MM] ^ (x>>1) ^ mag01[(int)(x&1ULL)];
                }
                for (;i<NN-1;i++) {
                        x = (mt[i]&UM)|(mt[i+1]&LM);
                        mt[i] = mt[i+(MM-NN)] ^ (x>>1) ^ mag01[(int)(x&1ULL)];
                }
                x = (mt[NN-1]&UM)|(mt[0]&LM);
                mt[NN-1] = mt[MM-1] ^ (x>>1) ^ mag01[(int)(x&1ULL)];

                mti = 0;
        }

        x = mt[mti++];

        x ^= (x >> 29) & 0x5555555555555555ULL;
        x ^= (x << 17) & 0x71D67FFFEDA60000ULL;
        x ^= (x << 37) & 0xFFF7EEE000000000ULL;
        x ^= (x >> 43);

        return x;
}

long long get_current_utime(void)
{
        struct timeval current;

        gettimeofday(&current,NULL);

        return (current.tv_sec*1000000 + current.tv_usec);
}

int main(int argc, char* argv[])
{
	int i;
	int temp;
	long long test_time = 0;

	n_objects		= atoi(argv[1]);
	object_size_1k 		= atoi(argv[2]);	// total object size (kbyte)
	hot_ratio		= 20;
	n_updates		= 500;
//	hot_ratio		= atoi(argv[3]);
//	n_updates		= atoi(argv[4]);
	object_size 		= object_size_1k * 1024;	// object size for each threads

	n_hot_objects		= (n_objects * hot_ratio)/100;
	n_cold_objects		= n_objects - n_hot_objects;

	printf("[POS TEST] Start Test for %d objects (hot:%d, cold:%d), %d Kbyte)\n", n_objects, n_hot_objects, n_cold_objects, object_size_1k);

	init_test_object_names();
	printf("\t--Init object names complete!\n");

#ifdef POS
	create_test_objects();
	printf("\t--Create objects complete! \n");

	alloc_memory_to_test_objects();
	printf("\t--Alloc memory complete! \n");

	test_time = update_test_objects();
	printf("\t--Update objects complete! \n");

	printf("Type something for continue..\n");
	scanf("%d",&temp);

	delete_test_objects();
	printf("\t--Delete test objects complete! \n");
#endif

	/* Free memory for name array */
        for(i=0; i<n_objects; i++){
                free(arr_object_name[i]);
        }
	free(arr_object_name);

	printf("[POS Test] Test end. Execution time: %lld usec\n", test_time);
	return 0;
}

void init_test_object_names(void)
{
	int i;
	char object_name[N_OBJECT_NAME];
	unsigned long long big_rand;

	/* Allocate Memory for object name variable */
	arr_object_name = (char**)malloc(sizeof(object_name) * n_objects);
	for(i=0; i<n_objects; i++){
		arr_object_name[i] = (char*)malloc(sizeof(object_name));
	}

	/* Allocate Memory for hot object name variable */
	arr_hot_object = (char**)malloc(sizeof(object_name) * n_hot_objects);
	for(i=0; i<n_hot_objects; i++){
		arr_hot_object[i] = (char*)malloc(sizeof(object_name));
	}

	/* Allocate Memory for hot object name variable */
	arr_cold_object = (char**)malloc(sizeof(object_name) * n_cold_objects);
	for(i=0; i<n_cold_objects; i++){
		arr_cold_object[i] = (char*)malloc(sizeof(object_name));
	}

	p_hot_object = (char**)malloc(sizeof(char*) * n_hot_objects);
	for(i=0; i<n_hot_objects; i++){
		p_hot_object[i] = (char*)malloc(sizeof(char*));
	}

	p_cold_object = (char**)malloc(sizeof(char*) * n_cold_objects);
	for(i=0; i<n_cold_objects; i++){
		p_cold_object[i] = (char*)malloc(sizeof(char*));
	}

	for(i=0; i<n_hot_objects; i++){
		/* Reset object name */
		memset(object_name, '\0', sizeof(object_name));

		/* Make object name*/
		sprintf(object_name,"h%d",i+1);
	
		/* Update object name array */
		strcpy(arr_hot_object[i], object_name);
		strcpy(arr_object_name[i], object_name);
	}

	for(i=0; i<n_cold_objects; i++){
		/* Reset object name */
		memset(object_name, '\0', sizeof(object_name));

		/* Make object name*/
		sprintf(object_name,"c%d",i+1);

		/* Update object name array */
		strcpy(arr_cold_object[i], object_name);
		strcpy(arr_object_name[i+n_hot_objects], object_name);
	}

	/* pre-compute random sequence based on 
                Fischer-Yates (Knuth) card shuffle */
        for(i = 0; i < n_objects; i++) {
                big_rand = genrand64_int64();
                big_rand = big_rand % n_objects;
		strcpy(object_name, arr_object_name[i]);
		strcpy(arr_object_name[i], arr_object_name[big_rand]);
                strcpy(arr_object_name[big_rand], object_name);
        }

//TEMP
/*
	for(i=0; i<n_objects; i++){
		printf("[%s]", arr_object_name[i]);
		if(arr_object_name[i][0] == 'c'){
			printf(" cold!\n");
		}
		else{
			printf(" hot!\n");
		}
	}
*/
}

#ifdef POS
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
	int hot = 0;
	int cold = 0;
	char* buf = (char*)malloc(object_size);
	memset(buf, 0xcafe, object_size);

	for(i = 0; i<n_objects; i++){
		if(arr_object_name[i][0] == 'c'){
			p_cold_object[cold] = (char*)pos_malloc(arr_object_name[i], object_size);
			if(p_cold_object[cold] == NULL){
				printf("[BUG] %s / %d object alloc fail\n", arr_object_name[i], i);
			}
			else{
				memcpy(p_cold_object[cold], buf, object_size);
			}
			cold++;
		}
		else{
			p_hot_object[hot] = (char*)pos_malloc(arr_object_name[i], object_size);
			if(p_hot_object[hot] == NULL){
				printf("[BUG] %s / %d  object alloc fail\n", arr_object_name[i], i);
			}
			else{
				memcpy(p_hot_object[hot], buf, object_size);
			}
			hot++;
		}
	}

	free(buf);
}

long long update_test_objects(void)
{
	int i;
	int is_hot;
	int object_nb;
	long long io_time_start, io_time_end;
	char* buf = (char*)malloc(object_size);
        memset(buf, 0xefac, object_size);

	io_time_start = get_current_utime();

//	srand((unsigned)time(NULL));
	for(i=0; i<n_updates; i++){
		is_hot = rand()%100;

		if(is_hot <= HOT_ACCESS_RATIO){
			object_nb = rand()%n_hot_objects;
			memcpy(p_hot_object[object_nb], buf, object_size);
		}
		else{
			object_nb = rand()%n_cold_objects;
			memcpy(p_cold_object[object_nb], buf, object_size);
		}
	}

	io_time_end = get_current_utime();

	free(buf);

	return (io_time_end - io_time_start);
}

void delete_test_objects(void)
{
	int i;
	for(i = 0; i<n_objects; i++){
		pos_delete(arr_object_name[i]);
	}
}
#endif

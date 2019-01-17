#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <semaphore.h>
#include <pthread.h>

#define N 10
#define BUFSIZE 100
#define KEYWORDSIZE 128

void *func(void *args);

struct queue_state{
	int states[N];
};

struct res_queue{
	int results[BUFSIZE + 1];
	int in;
	int out;
	int index;
};

struct request_queue{
	char keywords[N + 1][KEYWORDSIZE];
	int in;
	int out;
	int indices[N + 1];
};

struct thread_arg{
	int index;
	char keyword[KEYWORDSIZE];
	char shm_name[KEYWORDSIZE];
	char inputfilename[KEYWORDSIZE];
	char sem_name[KEYWORDSIZE];
};

typedef struct{
	struct queue_state q_state;
	struct res_queue res_queue1;
	struct res_queue res_queue2;
	struct res_queue res_queue3;
	struct res_queue res_queue4;
	struct res_queue res_queue5;
	struct res_queue res_queue6;
	struct res_queue res_queue7;
	struct res_queue res_queue8;
	struct res_queue res_queue9;
	struct res_queue res_queue10;
	struct request_queue req_queue;
} my_shm;

//Main
int main(int argc, char *argv[]) {
	//Arguments
	char* shm_name = argv[1];
	char* inputfilename = argv[2];
	char* sem_name = argv[3];

	//Argument exceptions
	if(argc != 4){
		printf("Check the number of arguments!\n");
		return 1;
	}

	if(strlen(shm_name) > 128){
		printf("Re-enter shm_name!\n");
		return 1;
	}

	if(strlen(inputfilename) > 128){
		printf("Re-enter inputfilename!\n");
		return 1;
	}

	if(strlen(sem_name) > 128){
		printf("Re-enter sem_name!\n");
		return 1;
	}

	//Variables for semaphore
	sem_t *sem_server_wait;
	sem_t *sem_client;
	sem_t *sem_client_wait;
	sem_t *sem_line_wait;
	int sem_value_server_wait = 0; //Semaphore value for server wait
	int sem_value_client = 1; //Semaphore value for in out, mostly client related
	int sem_value_line_wait = 0;
	int sem_value_client_wait = 0;

	//Semaphore for server waiting client to set up request queue and queue status
	char* sem_name_post_server_wait = "_server_wait_cagatay_emre";
	char* sem_name_server_wait = (char *)malloc(strlen(sem_name) + strlen(sem_name_post_server_wait) + 1);
	strcpy(sem_name_server_wait, sem_name);
	strcat(sem_name_server_wait, sem_name_post_server_wait);

	//Semaphore for in and out assigning
	char* sem_name_post_client = "_client_cagatay_emre";
	char* sem_name_client = (char *)malloc(strlen(sem_name) + strlen(sem_name_post_client) + 1);
	strcpy(sem_name_client, sem_name);
	strcat(sem_name_client, sem_name_post_client);
	//printf("%s\n",sem_name_client);

	//Unlink previous semaphores if exist
	sem_unlink(sem_name_server_wait);
	sem_unlink(sem_name_client);

	//Initializing semaphore
	sem_server_wait = sem_open(sem_name_server_wait, O_CREAT | O_EXCL, 0666, sem_value_server_wait);
	if(sem_server_wait == SEM_FAILED){
		perror("semaphore");
		return 1;
	}

	sem_client = sem_open(sem_name_client, O_CREAT | O_EXCL, 0666, sem_value_client);
	if(sem_client == SEM_FAILED){
		perror("semaphore");
		return 1;
	}

	for(int index = 1; index < 11; index++){
		//Semaphore for client waiting server to get the data from result queue x 10
		char* sem_name_post_client_wait = "_client_wait_cagatay_emre";
		char* sem_name_client_wait = (char *)malloc(strlen(sem_name) + strlen(sem_name_post_client_wait) + sizeof(index) + 1);
		char* str_index = (char *)malloc(sizeof(index));
		sprintf(str_index, "%i", index);
		strcpy(sem_name_client_wait, sem_name);
		strcat(sem_name_client_wait, sem_name_post_client_wait);
		strcat(sem_name_client_wait, str_index);
		//printf("%s\n", sem_name_client_wait);

		//Semaphore for line x10
		char* sem_name_post_line_wait = "_line_wait_cagatay_emre";
		char* sem_name_line_wait = (char *)malloc(strlen(sem_name) + strlen(sem_name_post_line_wait) + sizeof(index) + 1);
		strcpy(sem_name_line_wait, sem_name);
		strcat(sem_name_line_wait, sem_name_post_line_wait);
		strcat(sem_name_line_wait, str_index);

		//Unlink previous semaphores if exist
		sem_unlink(sem_name_client_wait);

		//Initializing semaphore
		sem_client_wait = sem_open(sem_name_client_wait, O_CREAT | O_EXCL, 0666, sem_value_client_wait);
		if(sem_client_wait == SEM_FAILED){
			perror("semaphore");
			return 1;
		}

		sem_unlink(sem_name_line_wait);
		//Initializing semaphore
		sem_line_wait = sem_open(sem_name_line_wait, O_CREAT | O_EXCL, 0666, sem_value_line_wait);
		if(sem_line_wait == SEM_FAILED){
			perror("semaphore");
			exit(1);
		}
	}

	//Variables for shared memory
	int fd;
	my_shm *shmmap;

	//File description	
	fd = shm_open(shm_name, O_CREAT | O_RDWR | O_TRUNC, 0666);
	if(fd < 0){
		perror("open");
		return 1;
	}

	if(ftruncate(fd, sizeof(my_shm)) < 0){
		perror("ftruncate");
		return 1;
	}

	//Creating shared memory
	shmmap = mmap(NULL, sizeof(my_shm), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	
	if(shmmap == MAP_FAILED){
		perror("mmpa");
		close(fd);
		return 1;
	}

	//Initializing shared memory
	(shmmap->res_queue1).in = 0;
	(shmmap->res_queue1).out = 0;
	(shmmap->res_queue1).index = 1;
	(shmmap->res_queue2).in = 0;
	(shmmap->res_queue2).out = 0;
	(shmmap->res_queue2).index = 2;
	(shmmap->res_queue3).in = 0;
	(shmmap->res_queue3).out = 0;
	(shmmap->res_queue3).index = 3;
	(shmmap->res_queue4).in = 0;
	(shmmap->res_queue4).out = 0;
	(shmmap->res_queue4).index = 4;
	(shmmap->res_queue5).in = 0;
	(shmmap->res_queue5).out = 0;
	(shmmap->res_queue5).index = 5;
	(shmmap->res_queue6).in = 0;
	(shmmap->res_queue6).out = 0;
	(shmmap->res_queue6).index = 6;
	(shmmap->res_queue7).in = 0;
	(shmmap->res_queue7).out = 0;
	(shmmap->res_queue7).index = 7;
	(shmmap->res_queue8).in = 0;
	(shmmap->res_queue8).out = 0;
	(shmmap->res_queue8).index = 8;
	(shmmap->res_queue9).in = 0;
	(shmmap->res_queue9).out = 0;
	(shmmap->res_queue9).index = 9;
	(shmmap->res_queue10).in = 0;
	(shmmap->res_queue10).out = 0;
	(shmmap->res_queue10).index = 10;
	(shmmap->req_queue).in = 0;
	(shmmap->req_queue).out = 0;
	
	for(int i = 0; i < N; i++){
		(shmmap->q_state).states[i] = 0;
	}

	for(int i = 0; i < N; i++){
		(shmmap->req_queue).indices[i] = 0;
	}

	//Variables for thread
	pthread_t tid;
	pthread_attr_t attr;

	//Reading from request queue
	char keyword[KEYWORDSIZE];
	int index;
	while(1){
		//Server waits if there is not client in request queue
		if(sem_wait(sem_server_wait) < 0){
			perror("sem_wait");
			return 1;
		}

		strcpy(keyword, (shmmap->req_queue).keywords[(shmmap->req_queue).out]);
		index = (shmmap->req_queue).indices[(shmmap->req_queue).out];
		struct thread_arg *args = malloc(sizeof(struct thread_arg));
		args->index = index;
		strcpy(args->keyword, keyword);
		strcpy(args->shm_name, shm_name);
		strcpy(args->inputfilename, inputfilename);
		strcpy(args->sem_name, sem_name);

		(shmmap->req_queue).out = ((shmmap->req_queue).out + 1) % (N + 1);

		//Creating and executing threads
		pthread_attr_init(&attr); 
		pthread_create(&tid, &attr, func, args);
	}

	//Deallocating shared memory
	if(munmap(shmmap, sizeof(my_shm)) < 0){
		perror("munmap");
		return 1;
	}
	
	close(fd);

	return 0;
}

void *func(void *my_args){
	//Variables
	FILE *fp_text;
	struct thread_arg *args = my_args;

	int line = 1;
	int result = 0;
	char arr[1024];
	
	int index = args->index;
	char keyword[KEYWORDSIZE];
	char shm_name[KEYWORDSIZE];
	char inputfilename[KEYWORDSIZE];
	char sem_name[KEYWORDSIZE];

	strcpy(keyword, args->keyword);
	strcpy(shm_name, args->shm_name);
	strcpy(inputfilename, args->inputfilename);
	strcpy(sem_name, args->sem_name);

	//Variables for shared memory
	int fd;
	my_shm *shmmap;

	//File description	
	fd = shm_open(shm_name, O_RDWR, 0666);
	if(fd < 0){
		perror("open");
		exit(1);
	}

	//Semaphore
	sem_t *sem_client_wait;
	sem_t *sem_line_wait;
	int sem_value_client_wait = 0; //Semaphore value for client wait
	int sem_value_line_wait = 0;

	//Semaphore for client waiting server to get the data from result queue x 10
	char* sem_name_post_client_wait = "_client_wait_cagatay_emre";
	char* sem_name_client_wait = (char *)malloc(strlen(sem_name) + strlen(sem_name_post_client_wait) + sizeof(index) + 1);
	char* str_index = (char *)malloc(sizeof(index));
	sprintf(str_index, "%i", index);
	strcpy(sem_name_client_wait, sem_name);
	strcat(sem_name_client_wait, sem_name_post_client_wait);
	strcat(sem_name_client_wait, str_index);

	//Semaphore for line x10
	char* sem_name_post_line_wait = "_line_wait_cagatay_emre";
	char* sem_name_line_wait = (char *)malloc(strlen(sem_name) + strlen(sem_name_post_line_wait) + sizeof(index) + 1);
	strcpy(sem_name_line_wait, sem_name);
	strcat(sem_name_line_wait, sem_name_post_line_wait);
	strcat(sem_name_line_wait, str_index);

	//Initializing semaphore
	sem_client_wait = sem_open(sem_name_client_wait, O_CREAT, 0666, sem_value_client_wait);
	if(sem_client_wait == SEM_FAILED){
		perror("semaphore");
		exit(1);
	}

	//Initializing semaphore
	sem_line_wait = sem_open(sem_name_line_wait, O_CREAT, 0666, sem_value_line_wait);
	if(sem_client_wait == SEM_FAILED){
		perror("semaphore");
		exit(1);
	}

	//Opening shared memory
	shmmap = mmap(NULL, sizeof(my_shm), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if(shmmap == MAP_FAILED){
		perror("mmpa");
		close(fd);
		exit(1);
	}

	if((fp_text = fopen(inputfilename, "r")) == NULL){
		perror("fopen");
		exit(1);
	}

	while(fgets(arr, 1024, fp_text) != NULL) {
		if((strstr(arr, keyword)) != NULL) {
			//Not critical point, each uses different result queue and we do not deallocate queue state
			if(index == 1)
			{
				(shmmap->res_queue1).results[(shmmap->res_queue1).in] = line;
				(shmmap->res_queue1).in = ((shmmap->res_queue1).in + 1) % (BUFSIZE + 1);
			}
			else if(index == 2)
			{
				(shmmap->res_queue2).results[(shmmap->res_queue2).in] = line;
				(shmmap->res_queue2).in =((shmmap->res_queue2).in + 1) % (BUFSIZE + 1);
			}
			else if(index == 3)
			{
				(shmmap->res_queue3).results[(shmmap->res_queue3).in] = line;
				(shmmap->res_queue3).in =((shmmap->res_queue3).in + 1) % (BUFSIZE + 1);
			}
			else if(index == 4)
			{
				(shmmap->res_queue4).results[(shmmap->res_queue4).in] = line;
				(shmmap->res_queue4).in =((shmmap->res_queue4).in + 1) % (BUFSIZE + 1);
			}
			else if(index == 5)
			{
				(shmmap->res_queue5).results[(shmmap->res_queue5).in] = line;
				(shmmap->res_queue5).in =((shmmap->res_queue5).in + 1) % (BUFSIZE + 1);
			}
			else if(index == 6)
			{
				(shmmap->res_queue6).results[(shmmap->res_queue6).in] = line;
				(shmmap->res_queue6).in =((shmmap->res_queue6).in + 1) % (BUFSIZE + 1);
			}
			else if(index == 7)
			{
				(shmmap->res_queue7).results[(shmmap->res_queue7).in] = line;
				(shmmap->res_queue7).in =((shmmap->res_queue7).in + 1) % (BUFSIZE + 1);
			}
			else if(index == 8)
			{
				(shmmap->res_queue8).results[(shmmap->res_queue8).in] = line;
				(shmmap->res_queue8).in =((shmmap->res_queue8).in + 1) % (BUFSIZE + 1);
			}
			else if(index == 9)
			{
				(shmmap->res_queue9).results[(shmmap->res_queue9).in] = line;
				(shmmap->res_queue9).in =((shmmap->res_queue9).in + 1) % (BUFSIZE + 1);
			}
			else if(index == 10)
			{
				(shmmap->res_queue10).results[(shmmap->res_queue10).in] = line;
				(shmmap->res_queue10).in =((shmmap->res_queue10).in + 1) % (BUFSIZE + 1);
			}
			

			result++;
		}

		if(((result % 1) == 0) && result != 0){
			//Server is done with result queue
			if(sem_post(sem_client_wait) < 0){
				perror("sem_wait");
				exit(1);
			}

			if(sem_wait(sem_line_wait) < 0){
				perror("sem_wait");
				exit(1);
			}

			result = 0;
		}

		line++;
	}

	fclose(fp_text);

	if(index == 1)
	{
		(shmmap->res_queue1).results[(shmmap->res_queue1).in] = -1;
		(shmmap->res_queue1).in = ((shmmap->res_queue1).in + 1) % (BUFSIZE + 1);
	}
	else if(index == 2)
	{
		(shmmap->res_queue2).results[(shmmap->res_queue2).in] = -1;
		(shmmap->res_queue2).in =((shmmap->res_queue2).in + 1) % (BUFSIZE + 1);
	}
	else if(index == 3)
	{
		(shmmap->res_queue3).results[(shmmap->res_queue3).in] = -1;
		(shmmap->res_queue3).in =((shmmap->res_queue3).in + 1) % (BUFSIZE + 1);
	}
	else if(index == 4)
	{
		(shmmap->res_queue4).results[(shmmap->res_queue4).in] = -1;
		(shmmap->res_queue4).in =((shmmap->res_queue4).in + 1) % (BUFSIZE + 1);
	}
	else if(index == 5)
	{
		(shmmap->res_queue5).results[(shmmap->res_queue5).in] = -1;
		(shmmap->res_queue5).in =((shmmap->res_queue5).in + 1) % (BUFSIZE + 1);
	}
	else if(index == 6)
	{
		(shmmap->res_queue6).results[(shmmap->res_queue6).in] = -1;
		(shmmap->res_queue6).in =((shmmap->res_queue6).in + 1) % (BUFSIZE + 1);
	}
	else if(index == 7)
	{
		(shmmap->res_queue7).results[(shmmap->res_queue7).in] = -1;
		(shmmap->res_queue7).in =((shmmap->res_queue7).in + 1) % (BUFSIZE + 1);
	}
	else if(index == 8)
	{
		(shmmap->res_queue8).results[(shmmap->res_queue8).in] = -1;
		(shmmap->res_queue8).in =((shmmap->res_queue8).in + 1) % (BUFSIZE + 1);
	}
	else if(index == 9)
	{
		(shmmap->res_queue9).results[(shmmap->res_queue9).in] = -1;
		(shmmap->res_queue9).in =((shmmap->res_queue9).in + 1) % (BUFSIZE + 1);
	}
	else if(index == 10)
	{
		(shmmap->res_queue10).results[(shmmap->res_queue10).in] = -1;
		(shmmap->res_queue10).in =((shmmap->res_queue10).in + 1) % (BUFSIZE + 1);
	}

	if(sem_post(sem_client_wait) < 0){
		perror("sem_wait");
		exit(1);
	}

	if(sem_wait(sem_line_wait) < 0){
		perror("sem_wait");
		exit(1);
	}

	pthread_exit(0);
}

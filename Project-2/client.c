#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <semaphore.h>
#include <time.h>

#define N 10
#define BUFSIZE 100
#define KEYWORDSIZE 128

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
	char* keyword = argv[2];
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

	if(strlen(keyword) > 128){
		printf("Re-enter keyword!\n");
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
	int sem_value = 0; //Semaphore value will be ignored
	
	//Semaphore for server waiting client when request queue is empty
	char* sem_name_post_server_wait = "_server_wait_cagatay_emre";
	char* sem_name_server_wait = (char *)malloc(strlen(sem_name) + strlen(sem_name_post_server_wait) + 1);
	strcpy(sem_name_server_wait, sem_name);
	strcat(sem_name_server_wait, sem_name_post_server_wait);
	//printf("%s\n", sem_name_server_wait);

	//Semaphore for in and out assigning
	char* sem_name_post_client = "_client_cagatay_emre";
	char* sem_name_client = (char *)malloc(strlen(sem_name) + strlen(sem_name_post_client) + 1);
	strcpy(sem_name_client, sem_name);
	strcat(sem_name_client, sem_name_post_client);
	//printf("%s\n",sem_name_client);

	//Initializing semaphore
	//Semaphore for server waiting client when request queue is empty
	sem_server_wait = sem_open(sem_name_server_wait, O_CREAT, 0666, sem_value);
	if(sem_server_wait == SEM_FAILED){
		perror("sem_open");
		return 1;
	}

	//Semaphore for in and out assigning
	sem_client = sem_open(sem_name_client, O_CREAT, 0666, sem_value);
	if(sem_client == SEM_FAILED){
		perror("semaphore");
		return 1;
	}

	//Variables for shared memory
	int fd;
	my_shm *shmmap;

	//File description	
	fd = shm_open(shm_name, O_RDWR, 0666);
	if(fd < 0){
		perror("open");
		return 1;
	}

	//Opening shared memory
	shmmap = mmap(NULL, sizeof(my_shm), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if(shmmap == MAP_FAILED){
		perror("mmpa");
		close(fd);
		return 1;
	}

	//Load request queue one by one and changing in and out
	if(sem_wait(sem_client) < 0){
		perror("sem_wait");
		return 1;
	}

	//Loading to request queue
	//Critical Point 1
	int index;	//Result queue index & Semaphore index
	if(((shmmap->req_queue).in + 1) % (N + 1) == (shmmap->req_queue).out){
		printf("Request queue is full, try again later!\n");

		if(sem_post(sem_client) < 0){
			perror("sem_post");
			return 1;
		}

		return 1;
	}
	else{
		for(int i = 0; i < N; i++){
			if((shmmap->q_state).states[i] == 0){
				(shmmap->req_queue).indices[(shmmap->req_queue).in] = i + 1;
				index = i + 1;
				(shmmap->q_state).states[i] = 1;
				break;
			}
			else if(i == N - 1){
				printf("All result queues are full, try again later!\n");

				if(sem_post(sem_client) < 0){
					perror("sem_post");
					return 1;
				}
				return 1;
			}
		}

		strcpy((shmmap->req_queue).keywords[(shmmap->req_queue).in], keyword);
		(shmmap->req_queue).in = ((shmmap->req_queue).in + 1) % (N + 1);
	}

	if(sem_post(sem_client) < 0){
		perror("sem_post");
		return 1;
	}
	//Critical Point 1

	if(sem_post(sem_server_wait) < 0){
		perror("sem_post");
		return 1;
	}

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

	//Initializing semaphore
	sem_client_wait = sem_open(sem_name_client_wait, O_CREAT, 0666, sem_value);
	if(sem_client_wait == SEM_FAILED){
		perror("semaphore");
		return 1;
	}

	//Initializing semaphore
	sem_line_wait = sem_open(sem_name_line_wait, O_CREAT, 0666, sem_value);
	if(sem_line_wait == SEM_FAILED){
		perror("semaphore");
		exit(1);
	}

	while(1){
		//printf("index %i\n", index);
		//Client waits server to load result queue
		if(sem_wait(sem_client_wait) < 0){
			perror("sem_wait");
			return 1;
		}

		//Initializing semaphore
		sem_line_wait = sem_open(sem_name_line_wait, O_CREAT, 0666, sem_value);
		if(sem_client_wait == SEM_FAILED){
			perror("semaphore");
			exit(1);
		}

		int i = 0;

		//Critical Point 2
		//Getting from result queue
		if((shmmap->res_queue1).index == index){
			//printf("Line numbers for keyword \"%s\"\n", keyword);
			//printf("out%i\n", (shmmap->res_queue1).out);
			while(((shmmap->res_queue1).in) != (shmmap->res_queue1).out){
				i = (shmmap->res_queue1).results[(shmmap->res_queue1).out];
				if(i != -1){
					printf("%i\n", i);
				}
				(shmmap->res_queue1).out = ((shmmap->res_queue1).out + 1) % (BUFSIZE + 1);
			}
		}
		else if((shmmap->res_queue2).index == index){
			//printf("Line numbers for keyword \"%s\"\n", keyword);
			while(((shmmap->res_queue2).in) != (shmmap->res_queue2).out){
				i = (shmmap->res_queue2).results[(shmmap->res_queue2).out];
				if(i != -1){
					printf("%i\n", i);
				}
				(shmmap->res_queue2).out = ((shmmap->res_queue2).out + 1) % (BUFSIZE + 1);
			}
		}
		else if((shmmap->res_queue3).index == index){
			//printf("Line numbers for keyword \"%s\"\n", keyword);
			while(((shmmap->res_queue3).in) != (shmmap->res_queue3).out){
				i = (shmmap->res_queue3).results[(shmmap->res_queue3).out];
				if(i != -1){
					printf("%i\n", i);
				}
				(shmmap->res_queue3).out = ((shmmap->res_queue3).out + 1) % (BUFSIZE + 1);
			}
		}
		else if((shmmap->res_queue4).index == index){
			//printf("Line numbers for keyword \"%s\"\n", keyword);
			while(((shmmap->res_queue4).in) != (shmmap->res_queue4).out){
				i = (shmmap->res_queue4).results[(shmmap->res_queue4).out];
				if(i != -1){
					printf("%i\n", i);
				}
				(shmmap->res_queue4).out = ((shmmap->res_queue4).out + 1) % (BUFSIZE + 1);
			}
		}
		else if((shmmap->res_queue5).index == index){
			//printf("Line numbers for keyword \"%s\"\n", keyword);
			while(((shmmap->res_queue5).in) != (shmmap->res_queue5).out){
				i = (shmmap->res_queue5).results[(shmmap->res_queue5).out];
				if(i != -1){
					printf("%i\n", i);
				}
				(shmmap->res_queue5).out = ((shmmap->res_queue5).out + 1) % (BUFSIZE + 1);
			}
		}
		else if((shmmap->res_queue6).index == index){
			//printf("Line numbers for keyword \"%s\"\n", keyword);
			while(((shmmap->res_queue6).in) != (shmmap->res_queue6).out){
				i = (shmmap->res_queue6).results[(shmmap->res_queue6).out];
				if(i != -1){
					printf("%i\n", i);
				}
				(shmmap->res_queue6).out = ((shmmap->res_queue6).out + 1) % (BUFSIZE + 1);
			}
		}
		else if((shmmap->res_queue7).index == index){
			//printf("Line numbers for keyword \"%s\"\n", keyword);
			while(((shmmap->res_queue7).in) != (shmmap->res_queue7).out){
				i = (shmmap->res_queue7).results[(shmmap->res_queue7).out];
				if(i != -1){
					printf("%i\n", i);
				}
				(shmmap->res_queue7).out = ((shmmap->res_queue7).out + 1) % (BUFSIZE + 1);
			}
		}
		else if((shmmap->res_queue8).index == index){
			//printf("Line numbers for keyword \"%s\"\n", keyword);
			while(((shmmap->res_queue8).in) != (shmmap->res_queue8).out){
				i = (shmmap->res_queue8).results[(shmmap->res_queue8).out];
				if(i != -1){
					printf("%i\n", i);
				}
				(shmmap->res_queue8).out = ((shmmap->res_queue8).out + 1) % (BUFSIZE + 1);
			}
		}
		else if((shmmap->res_queue9).index == index){
			//printf("Line numbers for keyword \"%s\"\n", keyword);
			while(((shmmap->res_queue9).in) != (shmmap->res_queue9).out){
				i = (shmmap->res_queue9).results[(shmmap->res_queue9).out];
				if(i != -1){
					printf("%i\n", i);
				}
				(shmmap->res_queue9).out = ((shmmap->res_queue9).out + 1) % (BUFSIZE + 1);
			}
		}
		else if((shmmap->res_queue10).index == index){
			//printf("Line numbers for keyword \"%s\"\n", keyword);
			while(((shmmap->res_queue10).in) != (shmmap->res_queue10).out){
				i = (shmmap->res_queue10).results[(shmmap->res_queue10).out];
				if(i != -1){
					printf("%i\n", i);
				}
				(shmmap->res_queue10).out = ((shmmap->res_queue10).out + 1) % (BUFSIZE + 1);
			}
		}

		if(sem_post(sem_line_wait) < 0){
			perror("sem_post");
			return 1;
		}

		if(i == -1){
			(shmmap->q_state).states[index - 1] = 0;
			break;
		}
	}
	//Critical Point 2

	return 0;
}

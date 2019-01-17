#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <fcntl.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

float stdv(int * arr, int size);
float meanSum(int * arr, int size);
float average(int * arr, int size);
void fcfs(int * time_number,int * cylinder_number, int size_time, int current_cylinder);
void sstf(int * time_number,int * cylinder_number, int size_time, int default_cylinder);
void clook(int * time_number,int * cylinder_number, int size_time, int default_cylinder, int n);
void look(int * time_number,int * cylinder_number, int size_time, int default_cylinder, int n);

int main(int argc, char *argv[]) {
	//Arguments
	int n = atoi(argv[1]);
	char* filename = argv[2];

	//Argument exceptions
	if(argc != 3){
		printf("Check the number of arguments!\n");
		return 1;
	}

	if(n < 1){
		printf("N must be greater than 0!\n");
	}

	//Reading from a file
	FILE *fp_text;
	char arr[1024];

	if((fp_text = fopen(filename, "r")) == NULL){
		perror("fopen");
		exit(1);
	}

	//Reading from the file
	int line_number = 0;

	while(fgets(arr, 1024, fp_text) != NULL){
		line_number++;
	}

	fclose(fp_text);

	if((fp_text = fopen(filename, "r")) == NULL){
		perror("fopen");
		exit(1);
	}

	int *arr_1 = malloc(line_number * sizeof(int));
	int *arr_2 = malloc(line_number * sizeof(int));

	int index = 0;
	while(fgets(arr, 1024, fp_text) != NULL){
		sscanf(arr, "%i %i", &arr_1[index], &arr_2[index]);
		//printf("%i %i\n", arr_1[index], arr_2[index]);
		if(arr_2[index] > n){
			printf("N and Infile.txt do not match, please enter greater N number!\n");
			free(arr_1);
			free(arr_2);
			return 1;
		}
		index++;
	}

	fcfs(arr_1, arr_2, line_number, 1);
	sstf(arr_1, arr_2, line_number, 1);
	look(arr_1, arr_2, line_number, 1, n);
	clook(arr_1, arr_2, line_number, 1, n);

	free(arr_1);
	free(arr_2);

	return 0;
}

void fcfs(int * time_number,int * cylinder_number, int size_time, int default_cylinder)
{
	int current_cylinder = default_cylinder;
	int current_time = time_number[0];
	int finished = size_time;
	int *wait_time = malloc(size_time * sizeof(int));
	int wait_time_index = 0;

	int *time = malloc(size_time * sizeof(int));
	for(int i = 0; i < size_time; i++){
		time[i] = time_number[i];
	}

	while(finished != 0){
		int smallest = -1;
		int index = -1;
		for(int i = 0; i < size_time; i++){
			if(time[i] != -1){
				if(smallest == -1){
					smallest = time[i];
					index = i;
				}
				else if(time[i] < smallest){
					smallest = time[i];
					index = i;
				}
			}
		}

		if(smallest <= current_time && smallest != -1){
			//printf("I am %i", cylinder_number[index]);
			//printf(" Wait time is %i\n", current_time - time[index]);
			wait_time[wait_time_index] = current_time - time[index];
			wait_time_index++;
			while(current_cylinder != cylinder_number[index]){
				if(cylinder_number[index] < current_cylinder){
					current_cylinder--;
				}
				else if(current_cylinder < cylinder_number[index]){
					current_cylinder++;
				}
				current_time++;
			}

			if(current_cylinder == cylinder_number[index]){
				time[index] = -1;
				finished--;
				current_time--;
			}
		}
		current_time++;
	}
	printf("FCFS : %i %f %f\n", current_time, average(wait_time, size_time) , stdv(wait_time, size_time));
	free(wait_time);
	free(time);
}

void sstf(int * time_number,int * cylinder_number, int size_time, int default_cylinder)
{
	int current_cylinder = default_cylinder;
	int current_time = time_number[0];
	int finished = size_time;
	int *wait_time = malloc(size_time * sizeof(int));
	int wait_time_index = 0;

	int *time = malloc(size_time * sizeof(int));
	for(int i = 0; i < size_time; i++){
		time[i] = time_number[i];
	}

	while(finished != 0){
		int closest = -1;
		int index = -1;
		for(int i = 0; i < size_time; i++){
			if(time[i] != -1 && time[i] <= current_time){
				if(closest == -1){
					closest = abs(cylinder_number[i] - current_cylinder);
					index = i;
				}
				else if(abs(cylinder_number[i] - current_cylinder) < closest){
					closest = abs(cylinder_number[i] - current_cylinder);
					index = i;
				}
			}
		}

		if(closest != -1){
			//printf("I am %i", cylinder_number[index]);
			//printf(" Wait time is %i\n", current_time - time[index]);
			wait_time[wait_time_index] = current_time - time[index];
			wait_time_index++;
			while(current_cylinder != cylinder_number[index]){
				if(cylinder_number[index] < current_cylinder){
					current_cylinder--;
				}
				else if(current_cylinder < cylinder_number[index]){
					current_cylinder++;
				}
				current_time++;
			}

			if(current_cylinder == cylinder_number[index]){
				time[index] = -1;
				finished--;
				current_time--;
			}
		}
		current_time++;
	}
	printf("SSTF : %i %f %f\n", current_time, average(wait_time, size_time) , stdv(wait_time, size_time));
	free(wait_time);
	free(time);
}

void clook(int * time_number,int * cylinder_number, int size_time, int default_cylinder, int n)
{
	int current_cylinder = default_cylinder;
	int current_time = time_number[0];
	int finished = size_time;
	int *wait_time = malloc(size_time * sizeof(int));
	int direction = 1; //0 indicates left & 1 indicates right
	int smallest_index = -1;
	int biggest_index = -1;
	int next_cylinder_index = -1;
	int extra_waiting = 0;

	int *time = malloc(size_time * sizeof(int));
	for(int i = 0; i < size_time; i++){
		time[i] = time_number[i];
	}

	/*if((n - default_cylinder) > default_cylinder){
		direction = 0;
	}
	else{
		direction = 1;
	}*/

	while(finished != 0){
		smallest_index = -1;
		biggest_index = -1;
		next_cylinder_index = -1;
		for(int i = 0; i < size_time; i++){
			if(time[i] != -1 && time[i] <= current_time){
				if(direction == 0){
					if(next_cylinder_index == -1 && cylinder_number[i] <= current_cylinder){
						next_cylinder_index = i;
					}
					else if(cylinder_number[i] <= current_cylinder){
						if((current_cylinder - cylinder_number[i]) <= (current_cylinder - cylinder_number[next_cylinder_index])){
							next_cylinder_index = i;
						}
					}
				}
				else if(direction == 1){
					if(next_cylinder_index == -1 && cylinder_number[i] >= current_cylinder){
						next_cylinder_index = i;
					}
					else if(cylinder_number[i] >= current_cylinder){
						if((cylinder_number[i] - current_cylinder) <= (cylinder_number[next_cylinder_index] - current_cylinder)){
							next_cylinder_index = i;
						}
					}

				}

			}
		}
		if(next_cylinder_index ==  -1){
			for(int i = 0; i < size_time; i++){
				if(time[i] != -1 && time[i] <= current_time){
					if(direction == 0){
						if(biggest_index == -1 && cylinder_number[i] > current_cylinder){
							biggest_index = i;
						}
						else if(cylinder_number[i] > current_cylinder){
							if((cylinder_number[i] - current_cylinder) > (cylinder_number[biggest_index] - current_cylinder)){
								biggest_index = i;
							}
						}
						next_cylinder_index = biggest_index;
					}
					else if(direction == 1){
						if(smallest_index == -1 && cylinder_number[i] < current_cylinder){
							smallest_index = i;
						}
						else if(cylinder_number[i] < current_cylinder){
							if((current_cylinder - cylinder_number[i]) > (current_cylinder - cylinder_number[smallest_index])){
								smallest_index = i;
							}
						}
						next_cylinder_index = smallest_index;
					}
				}
			}
		}

		if(next_cylinder_index == -1){
			current_time++;
			extra_waiting++;
		}
		else{
			//printf("I am %i", cylinder_number[next_cylinder_index]);
			//printf("Wait time is %i\n", current_time - time[next_cylinder_index]);
			wait_time[next_cylinder_index] = current_time - time[next_cylinder_index];
			while(current_cylinder != cylinder_number[next_cylinder_index]){
				if(cylinder_number[next_cylinder_index] < current_cylinder){
					current_cylinder--;
				}
				else if(current_cylinder < cylinder_number[next_cylinder_index]){
					current_cylinder++;
				}
				current_time++;
			}

			if(current_cylinder == cylinder_number[next_cylinder_index]){
				time[next_cylinder_index] = -1;
				finished--;
			}
		}
	}

	printf("CLOOK: %i %f %f\n", current_time, average(wait_time, size_time) , stdv(wait_time, size_time));
	free(wait_time);
	free(time);
}

void look(int * time_number,int * cylinder_number, int size_time, int default_cylinder, int n)
{
	int current_cylinder = default_cylinder;
	int current_time = time_number[0];
	int finished = size_time;
	int *wait_time = malloc(size_time * sizeof(int));
	int direction = 1; //0 indicates left & 1 indicates right
	int smallest_index = -1;
	int biggest_index = -1;
	int next_cylinder_index = -1;
	int extra_waiting = 0;
	int *time = malloc(size_time * sizeof(int));
	for(int i = 0; i < size_time; i++){
		time[i] = time_number[i];
	}

	/*if((n - default_cylinder) > default_cylinder){
		direction = 0;
	}
	else{
		direction = 1;
	}*/

	while(finished != 0){
		smallest_index = -1;
		biggest_index = -1;
		next_cylinder_index = -1;
		for(int i = 0; i < size_time; i++){
			if(time[i] != -1 && time[i] <= current_time){
				if(direction == 0){
					if(next_cylinder_index == -1 && cylinder_number[i] <= current_cylinder){
						next_cylinder_index = i;
					}
					else if(cylinder_number[i] <= current_cylinder){
						if((current_cylinder - cylinder_number[i]) <= (current_cylinder - cylinder_number[next_cylinder_index])){
							next_cylinder_index = i;
						}
					}
				}
				else if(direction == 1){
					if(next_cylinder_index == -1 && cylinder_number[i] >= current_cylinder){
						next_cylinder_index = i;
					}
					else if(cylinder_number[i] >= current_cylinder){
						if((cylinder_number[i] - current_cylinder) <= (cylinder_number[next_cylinder_index] - current_cylinder)){
							next_cylinder_index = i;
						}
					}

				}

			}
		}
		if(next_cylinder_index ==  -1){	
					if(direction == 0){
						direction = 1;
						for(int i = 0; i < size_time; i++){
			if(time[i] != -1 && time[i] <= current_time){
				if(direction == 0){
					if(next_cylinder_index == -1 && cylinder_number[i] <= current_cylinder){
						next_cylinder_index = i;
					}
					else if(cylinder_number[i] <= current_cylinder){
						if((current_cylinder - cylinder_number[i]) <= (current_cylinder - cylinder_number[next_cylinder_index])){
							next_cylinder_index = i;
						}
					}
				}
				else if(direction == 1){
					if(next_cylinder_index == -1 && cylinder_number[i] >= current_cylinder){
						next_cylinder_index = i;
					}
					else if(cylinder_number[i] >= current_cylinder){
						if((cylinder_number[i] - current_cylinder) <= (cylinder_number[next_cylinder_index] - current_cylinder)){
							next_cylinder_index = i;
						}
					}

				}

			}
		}

					}
					else if(direction == 1){
						direction = 0;
						for(int i = 0; i < size_time; i++){
			if(time[i] != -1 && time[i] <= current_time){
				if(direction == 0){
					if(next_cylinder_index == -1 && cylinder_number[i] <= current_cylinder){
						next_cylinder_index = i;
					}
					else if(cylinder_number[i] <= current_cylinder){
						if((current_cylinder - cylinder_number[i]) <= (current_cylinder - cylinder_number[next_cylinder_index])){
							next_cylinder_index = i;
						}
					}
				}
				else if(direction == 1){
					if(next_cylinder_index == -1 && cylinder_number[i] >= current_cylinder){
						next_cylinder_index = i;
					}
					else if(cylinder_number[i] >= current_cylinder){
						if((cylinder_number[i] - current_cylinder) <= (cylinder_number[next_cylinder_index] - current_cylinder)){
							next_cylinder_index = i;
						}
					}

				}

			}
		}
					}		
		}

		if(next_cylinder_index == -1){
			current_time++;
			extra_waiting++;
		}
		else{
			//printf("I am %i", cylinder_number[next_cylinder_index]);
			//printf("Wait time is %i\n", current_time - time[next_cylinder_index]);
			wait_time[next_cylinder_index] = current_time - time[next_cylinder_index];
			while(current_cylinder != cylinder_number[next_cylinder_index]){
				if(cylinder_number[next_cylinder_index] < current_cylinder){
					current_cylinder--;
				}
				else if(current_cylinder < cylinder_number[next_cylinder_index]){
					current_cylinder++;
				}
				current_time++;
			}

			if(current_cylinder == cylinder_number[next_cylinder_index]){
				time[next_cylinder_index] = -1;
				finished--;
			}
		}
	}

	printf("LOOK : %i %f %f\n", current_time, average(wait_time, size_time) , stdv(wait_time, size_time));
	free(wait_time);
	free(time);
}

float stdv(int * arr, int size)
{
	float mean = 0;	
	for(int i = 0; i < size; i++){
		mean = mean + arr[i];
	}
	mean = mean / size;

	float variance = 0;
	for(int i = 0; i < size; i++){
		variance = variance + pow((arr[i] - mean), 2);
	}
	variance = variance / (size - 1);

	variance = sqrt(variance);

	return variance;
}

float meanSum(int * arr, int size)
{
	float sum = 0;

	for(int i = 0; i < size; i++){
		sum = sum + arr[i];
	}

	return sum / size;
}

float average(int * arr, int size)
{
	float sum = 0;

	for(int i = 0; i < size; i++)
	{
		sum = sum + arr[i];
	}

	return sum / size;
}

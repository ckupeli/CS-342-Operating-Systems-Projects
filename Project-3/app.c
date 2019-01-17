/*
	Çağatay Küpeli
	Emre Gürçay
	CS 342
	Project 3
	app.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define size 100000

#define MALLOC(s,t) if(((s) = malloc(t)) == NULL) {; }
//Main

unsigned long long int factorial(unsigned int i) {

   if(i <= 1) {
      return 1;
   }
   printf("Factorial %d\n", i);
   return i * factorial(i - 1);
}


int main() {

	int alloc_size = 0;
	int dealloc_size = 0;
	int yes = 0;
	int a = 0;
	int* j;
    unsigned long long int result;
    int **xyz;
    int k;


	printf("Allocating memory of size: %d\n", alloc_size);

	do{
		printf( "To allcote memory press 1 \n");
        printf( "To deallcote array press 2\n");
        printf( "For stack press 3\n");
        printf( "To allocate array press 4\n");
        printf( "To quit press something else\n");
		scanf("%d", &yes);
		if(yes == 1)
		{
			yes = 0;
			printf( "Enter a value :");
			scanf("%d", &alloc_size);
			printf( "You entered %d\n", alloc_size);
			while(a < alloc_size)
			{
				j = malloc(sizeof(int));
				printf("%d\n", a);
				a++;
			}
			a = 0;
		}
		else if(yes == 2)
		{

            alloc_size = 1;
            for (int i = 0; i < size; i++) {

                    free(xyz[i]);

            }
            free(xyz);
            yes = 0;
		}
		else if(yes == 3)
        {
            yes = 0;
            printf( "Enter a value :");
			scanf("%d", &alloc_size);
			printf( "You entered %d\n", alloc_size);
            result = factorial(alloc_size);
            //printf("Factorial of %d is %lld\n", alloc_size, factorial(alloc_size));
        }
        else if(yes == 4)
        {
            alloc_size = 1;
            MALLOC(xyz, sizeof(int *) * size);
                for (k = 0; k < size; k++) {
                    MALLOC(xyz[k], sizeof(int) * size);
                }
            yes = 0;
        }
        else
		{
			alloc_size = 0;
		}
	}while(alloc_size > 0);


	return 0;
}

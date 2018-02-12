#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define NUMBER_OF_INTS 40

typedef struct {
	int from_index;
	int to_index;
	int threadnum;
	int *array_to_sort;
} parameters;


void *bubblesort (void *params){
	parameters *range = params;
	printf("Thread %i has to sort from %i to %i\n",range->threadnum,range->from_index,range->to_index);
	
	int top = range->to_index;
	int bottom = range->from_index;
	int *to_sort = range->array_to_sort;
	free(range);
	
	int i = bottom;
	while (i < top){
		int j = bottom;
		while (j < top-i){
			if (to_sort[j] > to_sort[j+1]){
				int temp = to_sort[j+1];
				to_sort[j+1] = to_sort[j];
				to_sort[j] = temp;
			}
			j++;
		}
		i++;
	}
	return 0;
}

void *merge (void *params){
	
}

int main(void){
	int to_sort[NUMBER_OF_INTS] = {75, 25, 100, 24, 19, 67, 45, 88, 70, 96, 41, 36, 72, 150, 125, 25, 77, 200, 90, 166, 139, 55, 31, 8, 29, 119, 126, 137, 34, 62, 135, 121, 108, 197, 45, 35, 24, 1, 16, 43};
	
	printf("Array is:\n");
	int i = 0;
	while(i < NUMBER_OF_INTS)
		printf("%i\n",to_sort[i++]);
		
	pthread_t sorts[2];

	parameters *params = malloc(sizeof(parameters));
	
	params->from_index = 0;
	params->to_index = NUMBER_OF_INTS/2-1;
	params->threadnum = 0;
	params->array_to_sort = to_sort;
	
	if(pthread_create(&sorts[0], NULL, bubblesort, params)){
		printf("Couldn't start thread 0!\n");
		free(params);
		return -1;
	}
	
	params = malloc(sizeof(parameters));
	params->from_index = NUMBER_OF_INTS/2;
	params->to_index = NUMBER_OF_INTS-1;
	params->threadnum = 1;
	params->array_to_sort = to_sort;
	
	if(pthread_create(&sorts[1], NULL, bubblesort, params)){
		printf("Couldn't start thread 1!\n");
		free(params);
		return -1;
	}
	
	pthread_join(sorts[0], NULL);
	printf("Thread 0 is done!\n");
	
	pthread_join(sorts[1], NULL);
	printf("Thread 1 is done!\n");
	
	printf("All done!\n");
	printf("Current state of the array is:\n");
	
	i = 0;
	while (i < NUMBER_OF_INTS)
		printf("%i\n",to_sort[i++]);
	
	return 0;
}


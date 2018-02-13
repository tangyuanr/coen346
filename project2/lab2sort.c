#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#define NUMBER_OF_INTS 40

typedef struct {
/*	int from_index;
	int to_index;*/
	int threadnum;
	int *array_to_sort;
	int array_size;
} SortParameters;

typedef struct {
	int *lower_sub_array;
	int *upper_sub_array;
	int *destination_array;
	int array_size;
} MergeParameters;

void *bubblesort (void *parameters){
	SortParameters *params = parameters;
/*	printf("Thread %i has to sort from %i to %i\n",range->threadnum,range->from_index,range->to_index);

	int top = range->to_index;
	int bottom = range->from_index;*/
	int *to_sort = params->array_to_sort;
	int top = params->array_size-1;
	free(params);
/*	printf("Top is %i, Bottom is %i\n",top, bottom);

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
	}*/

	int i = 0;
	while (i < top){
		int j = 0;
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

void *merge (void *parameters){
	MergeParameters *params = parameters;

	int* lower = params->lower_sub_array;
	int* upper = params->upper_sub_array;
	int* dest = params->destination_array;
	int array_size = params->array_size;
	int subarray_size = array_size >> 1;

	int i = 0;
	int j = 0;
	int k = 0;

	while(i < array_size){
		if (j == subarray_size)
			dest[i++] = upper[k++];
		else if (k == subarray_size)
			dest[i++] = lower[j++];
		else{
			if (lower[j] < upper[k])
				dest[i++] = lower[j++];
			else
				dest[i++] = upper[k++];
		}
	}
	
	printf("Done merging! Array is now:\n");
	i = 0;
	while(i< array_size)
		printf("%i\n",dest[i++]);
	return 0;
}

int main(void){
	int to_sort[NUMBER_OF_INTS] = {75, 25, 100, 24, 19, 67, 45, 88, 70, 96, 41, 36, 72, 150, 125, 25, 77, 200, 90, 166, 139, 55, 31, 8, 29, 119, 126, 137, 34, 62, 135, 121, 108, 197, 45, 35, 24, 1, 16, 43};

	printf("Array is:\n");
	int i = 0;
	while(i < NUMBER_OF_INTS)
		printf("%i\n",to_sort[i++]);

	pthread_t sorts[3];

	SortParameters *sort_params = malloc(sizeof(SortParameters));
	sort_params->threadnum = 0;
	/*params->from_index = 0;
	params->to_index = NUMBER_OF_INTS/2-1;
	params->array_to_sort = to_sort;*/
	int lower_sub_array[NUMBER_OF_INTS/2];
	memcpy(lower_sub_array,to_sort,sizeof(lower_sub_array));
	sort_params->array_to_sort = lower_sub_array;
	sort_params->array_size = NUMBER_OF_INTS/2;

	if(pthread_create(&sorts[0], NULL, bubblesort, sort_params)){
		printf("Couldn't start thread 0!\n");
		free(sort_params);
		return -1;
	}

	sort_params = malloc(sizeof(SortParameters));
	sort_params->threadnum = 1;
	/*params->from_index = NUMBER_OF_INTS/2;
	params->to_index = NUMBER_OF_INTS-1;
	params->array_to_sort = to_sort;*/
	int upper_sub_array[NUMBER_OF_INTS/2];
	memcpy(upper_sub_array,&to_sort[NUMBER_OF_INTS/2],sizeof(upper_sub_array));
	sort_params->array_to_sort = upper_sub_array;
	sort_params->array_size = NUMBER_OF_INTS/2;

	if(pthread_create(&sorts[1], NULL, bubblesort, sort_params)){
		printf("Couldn't start thread 1!\n");
		free(sort_params);
		return -1;
	}

	pthread_join(sorts[0], NULL);
	//printf("Thread 0 is done!\n");

	pthread_join(sorts[1], NULL);
	//printf("Thread 1 is done!\n");

	printf("All done!\n");
	
	printf("Current state of the lower sub array is:\n");
	i = 0;
	while (i < NUMBER_OF_INTS/2)
		printf("%i\n",lower_sub_array[i++]);

	printf("Current state of the upper sub array is:\n");
	i = 0;
	while (i < NUMBER_OF_INTS/2)
		printf("%i\n",upper_sub_array[i++]);

	MergeParameters *merge_params = malloc(sizeof(MergeParameters));
	merge_params->lower_sub_array = lower_sub_array;
	merge_params->upper_sub_array = upper_sub_array;
	merge_params->destination_array = to_sort;
	merge_params->array_size = NUMBER_OF_INTS;

	if(pthread_create(&sorts[2], NULL, merge, merge_params)){
		printf("Couldn't start thread 2!\n");
		free(merge_params);
		return -1;
	}

	pthread_join(sorts[2], NULL);

		printf("Final sorted array is:\n");
	i = 0;
	while (i < NUMBER_OF_INTS)
		printf("%i\n",to_sort[i++]);


	return 0;
}


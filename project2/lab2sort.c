// Simple threaded sorting program that split an array in 2 equally sized sub-
// arrays and merge each in a separate thread before mergin them in another 
// thread. Array is hardcoded in a definition in main function.

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#define NUMBER_OF_INTS 40

typedef struct {
	int *array_to_sort;
	int array_size;
} SortParameters; // Will hold the parameters to the sort function

typedef struct {
	int *lower_sub_array;
	int *upper_sub_array;
	int *destination_array;
	int array_size;
} MergeParameters; // Will hold the parameters to the merge function

void *bubblesort (void *parameters){

	// Copy the received parameters to local variables
	SortParameters *params = parameters;
	int *to_sort = params->array_to_sort;
	int top = params->array_size-1;
	free(params);

	// Basic bubblesort. Comparaes adjacent vakues from the beginning of the
	// array, swapping them when left larger than right. until all are in order
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
	// Copy the received parameters to local variables
	MergeParameters *params = parameters;
	int* lower = params->lower_sub_array;
	int* upper = params->upper_sub_array;
	int* dest = params->destination_array;
	int array_size = params->array_size;
	int subarray_size = array_size >> 1;
	free(params);

	// Basic mergesort of both subarrays.
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

	return 0;
}

int main(void){
	// Declare all of the array from the values given in the project description
	int to_sort[NUMBER_OF_INTS] = {75, 25, 100, 24, 19, 67, 45, 88, 70, 96, 41, 36, 72, 150, 125, 25, 77, 200, 90, 166, 139, 55, 31, 8, 29, 119, 126, 137, 34, 62, 135, 121, 108, 197, 45, 35, 24, 1, 16, 43};

	// Show array on console
	printf("Array is:\n");
	int i = 0;
	while(i < NUMBER_OF_INTS)
		printf("%i\n",to_sort[i++]);

	// Split data of the aray in two parts that we copy in subarrays
	int lower_sub_array[NUMBER_OF_INTS >> 1];
	memcpy(lower_sub_array,to_sort,sizeof(lower_sub_array));
	int upper_sub_array[NUMBER_OF_INTS >> 1];
	memcpy(upper_sub_array,&to_sort[NUMBER_OF_INTS >> 1],sizeof(upper_sub_array));

	pthread_t sorts[3];

	// Set parameters for the bubblesort of the lower subarray
	SortParameters *sort_params = malloc(sizeof(SortParameters));
	sort_params->array_to_sort = lower_sub_array;
	sort_params->array_size = NUMBER_OF_INTS >> 1;

	// Start the thread to sort lower subarray
	if(pthread_create(&sorts[0], NULL, bubblesort, sort_params)){
		printf("Couldn't start thread 0!\n");
		free(sort_params);
		return -1;
	} // Returning with error if we can't start thread

	// Set parameters for the bubblesort of the upper subarray
	sort_params = malloc(sizeof(SortParameters));
	sort_params->array_to_sort = upper_sub_array;
	sort_params->array_size = NUMBER_OF_INTS >> 1;

	// Start the thread to sort lower subarray
	if(pthread_create(&sorts[1], NULL, bubblesort, sort_params)){
		printf("Couldn't start thread 1!\n");
		free(sort_params);
		return -1;
	} // Returning with error if we can't start thread

	// Join both threads to make sure both subarrays are sorted before continuing
	pthread_join(sorts[0], NULL);
	pthread_join(sorts[1], NULL);

	// Set parameters for the merging thread
	MergeParameters *merge_params = malloc(sizeof(MergeParameters));
	merge_params->lower_sub_array = lower_sub_array;
	merge_params->upper_sub_array = upper_sub_array;
	merge_params->destination_array = to_sort;
	merge_params->array_size = NUMBER_OF_INTS;


	// Start the thread to merge both subarrays
	if(pthread_create(&sorts[2], NULL, merge, merge_params)){
		printf("Couldn't start thread 2!\n");
		free(merge_params);
		return -1;
	} // Returning with error if we can't start thread

	// Wait for the merging to finish before continuing
	pthread_join(sorts[2], NULL);

	// Then print the sorted array
	printf("Final sorted array is:\n");
	i = 0;
	while (i < NUMBER_OF_INTS)
		printf("%i\n",to_sort[i++]);

	return 0;
}


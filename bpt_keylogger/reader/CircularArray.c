#include "math.h"

#define ARR_LEN 128


typedef struct {
	int data[ARR_LEN];
	int first;
	int last;
	int size;
	int sum;
} CircularArray;


void init(CircularArray* arr){
	arr->first = -1;
	arr->last = -1;
	arr->size = 0;
	arr->sum = 0;
	int i = 0;
	for(i = 0; i < ARR_LEN; ++i) arr->data[i] = 0;
}

int shift(CircularArray* arr, int newInt){
//Removes first element of arr, inserts newInt. Updates first, last. 
	if(arr->first == -1){
	//Insert the first element
		arr->first = arr->last = 0;
		arr->data[0] = newInt;
		++arr->size;
		arr->sum += newInt;
		return -1; 
	}
	else if(arr->size == ARR_LEN){
	//If the array is full
		int removed;
		removed = arr->data[arr->first];
		if(arr->first > arr->last){
			arr->last = arr->first;
			if(arr->first == ARR_LEN-1) arr->first--;
		    else arr->first++;
			arr->data[arr->last] = newInt;
		}
		else {
			arr->last = arr->first;
			if(arr->first == 0) arr->first = ARR_LEN-1;  
			else arr->first--;
			arr->data[arr->last] = newInt;	
		}
		arr->sum += newInt;
		arr->sum -= removed;
		return removed;
	}
	else {
	//Insert an element when the array is non-full and non-empty. 
		arr->data[arr->last + 1] = newInt;
		++arr->last;
		++arr->size;
		arr->sum += newInt;
		return -1;
	}
}

/*
void printArray(CircularArray arr){
	int i = 0;
	printf("%s", "\n");
	for(i = 0; i < ARR_LEN; ++i){
		printf("%d, ", arr.data[i]);
	}
	printf("%s", "\n");
	printf("First: %d 	Last: %d \n", arr.first, arr.last);
}
*/

double averageDev(CircularArray* arr){
//Calculated the average deviation of the circular array
	int i = 0;
	double totalDeviation= 0;
	double mean = arr->sum / arr->size;
	for(i = 0; i < arr->size; ++i){
		totalDeviation += fabs(mean - (double)(arr->data[i]));
	}
	//printf("The mean was: %f and the total deviation was: %f \n", mean, totalDeviation);
	return totalDeviation / arr->size;
}

/*
int main(){
	CircularArray arr;
	init(&arr);
	int i = 0;
	for(i = 0; i < 20; ++i){
		shift(&arr, rand()/1000000);
		printf("%f\n", averageDev(&arr));
	}
	printArray(arr);
	return 0;
}
*/
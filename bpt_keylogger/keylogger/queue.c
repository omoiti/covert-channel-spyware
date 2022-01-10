#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

struct Node;

typedef struct Node{
	struct Node* next;
	const char* data;
} Node;

typedef struct {
	Node* head;
	Node* tail;
	pthread_mutex_t lock;
	pthread_cond_t empty;
} Queue;

Node* initNode(const char* data){
//Returns a new node, containing the data passed.
	Node *newNode = malloc(sizeof(Node));
	newNode->data = data;
	newNode->next = NULL;
	return newNode;
}

Queue* initQueue(){
//Returns a new empty Queue object
	Queue* queue = malloc(sizeof(Queue));
	queue->head = NULL;
	queue->tail = NULL;
	pthread_mutex_init( &(queue->lock), NULL);
	pthread_cond_init( &(queue->empty), NULL);
	return queue;
}

int enqueue(Queue* queue, const char* data){
	//If the queue is empty, insert the first element
//First, get the lock on the queue. 
	pthread_mutex_lock(&(queue->lock));
	int wasNull = 0;
	if((queue->tail == NULL) || (queue->head == NULL)){
		queue->tail = queue->head = initNode(data);
		wasNull = 1;
	}
	else{
		//Link the new node to the list
		queue->tail->next = initNode(data);
		//Set the new node to be the tail
		queue->tail = queue->tail->next;
	}
	pthread_mutex_unlock(&(queue->lock));
	if(wasNull){
		pthread_cond_signal(&(queue->empty));
	}
	return 1;
}

int dequeue(Queue* queue){
//"Pops" the head of the queue, setting the head to be the next element
// Will block if the queue is empty.
	//Get a lock on the queue
	pthread_mutex_lock(&(queue->lock));
	//While the queue is empty, wait on a condition variable 
	while(queue->head == NULL) {
		pthread_cond_wait(&(queue->empty), &(queue->lock));
	}
	//If there is only one element in the queue, set the tail to nullptr.
	if(queue->head == queue->tail) queue->tail = NULL;
	//Store the next element to be head, as current head is to be deleted
	Node* tmp = queue->head->next;
	//Unallocate the memory used by the element to be deleted
	free(queue->head);
	//Set the new head
	queue->head = tmp;
	pthread_mutex_unlock(&(queue->lock));
	return 1;
}

const char* head(Queue* queue){
//Accessor to the head of the queue, does not modify the queue
	pthread_mutex_lock(&(queue->lock));
	//While the queue is empty, wait on a condition variable 
	while(queue->head == NULL){
		pthread_cond_wait(&(queue->empty), &(queue->lock));	
	} 
	//Store the next element to be head, as current head is to be deleted
	const char* data = queue->head->data;
	pthread_mutex_unlock(&(queue->lock));
	return data;
}

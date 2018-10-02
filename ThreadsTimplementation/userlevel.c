#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>

#include "threads.h"

#define NUM_THREADS 8

pthread_mutex_t print_lock;

void *threadFunction(void * data) {
	int threadNumber = (int) data;

	while(1) {
		pthread_mutex_lock(&print_lock);
		printf("Hello from thread %d\n", threadNumber);
		fflush(stdout);
		pthread_mutex_unlock(&print_lock);

		blocking_call(5);
		thread_yield();
	}

	return NULL;
}

int main(void) {
	// Initialize the threading package (0 = no preemption; 1 = preemption)
	thread_init(0);

	// Initialize the mutex
	pthread_mutex_init(&print_lock, NULL);

	for(int i = 0; i < NUM_THREADS; i++) {
		thread_create(threadFunction, (void *) i);
	}

	// Join threads
	for(int i = 0; i < NUM_THREADS; i++) {
		thread_join(i);
	}

	return EXIT_SUCCESS;
}

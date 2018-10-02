#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

#include "threads.h"

#define NUM_THREADS 8

pthread_mutex_t print_lock;

void *threadFunction(void * data) {
	int threadNumber = (int) data;

	while(1) {
		pthread_mutex_lock(&print_lock);
		printf("Hello from thread %d\n", threadNumber);
		pthread_mutex_unlock(&print_lock);

		blocking_call(5);
	}

	return NULL;
}

int main(void) {
	pthread_t threadNumber[NUM_THREADS];
	int threadData[NUM_THREADS];

	for(int i = 0; i < NUM_THREADS; i++) {
		threadData[i] = i;
	}

	pthread_attr_t config;

	pthread_attr_init(&config);
	pthread_attr_setdetachstate(&config, PTHREAD_CREATE_JOINABLE);

	pthread_mutex_init(&print_lock, NULL);

	// Launches all threads -- they are automatically started
	for(int i = 0; i < NUM_THREADS; i++) {
		if(pthread_create(&threadNumber[i], &config, threadFunction, (void *) threadData[i]) != 0) {
			fprintf(stderr, "Error creating thread\n");

			return EXIT_FAILURE;
		}
	}

	// Blocks main thread until all others return

	void *status;

	for(int i = 0; i < NUM_THREADS; i++) {
		if(pthread_join(threadNumber[i], NULL) != 0) {
			fprintf(stderr, "Error joning threads\n");

			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}

#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

#define NUM_THREADS 128

int global = 0;

void *threadFunction(void * data) {
	global += 2;

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

	// Launches all threads -- they are automatically started
	for(int i = 0; i < NUM_THREADS; i++) {
		if(pthread_create(&threadNumber[i], &config, threadFunction, (void *) &threadData[i]) != 0) {
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

	printf("Global value is %d\n", global);

	return EXIT_SUCCESS;
}

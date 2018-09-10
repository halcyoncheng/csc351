// Use the -O3 option for GCC
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

#define NUM_THREADS 8

int done = 0;

void *producer(void * data) {
	done = 1;

	return NULL;
}

void *consumer(void * data) {
	while(!done) {} // spin!

	printf("Hello world from consumer\n");

	return NULL;
}

int main(void) {
 	pthread_t producerID;
 	pthread_t consumerID;

	pthread_attr_t config;

	pthread_attr_init(&config);
	pthread_attr_setdetachstate(&config, PTHREAD_CREATE_JOINABLE);

	pthread_create(&consumerID, &config, consumer, NULL);
	pthread_create(&producerID, &config, producer, NULL);

	pthread_join(consumerID, NULL);
	pthread_join(producerID, NULL);

	return EXIT_SUCCESS;
}

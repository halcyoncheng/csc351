// Use the -O3 option for GCC
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>
#include<stdatomic.h>

#define NUM_THREADS 8

atomic_int varA = 0;
atomic_int varB = 0;

void *threadA(void * data) {
	atomic_store(&varB, 42);

	printf("var A = %d\n", varA);

	return NULL;
}

void *threadB(void * data) {
	atomic_store(&varA, 42);

	printf("var B = %d\n", varB);

	return NULL;
}

int main(void) {
	printf("-------\n");

 	pthread_t threadAID;
 	pthread_t threadBID;

	pthread_attr_t config;

	pthread_attr_init(&config);
	pthread_attr_setdetachstate(&config, PTHREAD_CREATE_JOINABLE);

	pthread_create(&threadBID, &config, threadB, NULL);
	pthread_create(&threadAID, &config, threadA, NULL);

	pthread_join(threadBID, NULL);
	pthread_join(threadAID, NULL);

	return EXIT_SUCCESS;
}

#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<unistd.h>
#include<time.h>

pthread_mutex_t chopstick[5];

// See cppreference: this is 1000ns
struct timespec timeout = { 0, 1000 };

void *think_eat_sleep(void *argument) {
	// n: philosopher number
	int n = (int) (argument);

	while(1) {
		printf ("Philosopher %d is thinking\n", n);

		if(pthread_mutex_timedlock(&chopstick[n], &timeout) != 0) {
			continue;
		}
		if(pthread_mutex_timedlock(&chopstick[(n+1) % 5], &timeout) != 0) {
			pthread_mutex_unlock(&chopstick[n]);
			continue;
		}

		printf ("Philosopher %d got hungry. Eating...\n", n);

		// That's 100 miliseconds
		usleep(100000);

		pthread_mutex_unlock(&chopstick[n]);
		pthread_mutex_unlock(&chopstick[(n+1) % 5]);

		printf ("Philosopher %d is full. Sleeping zzz...\n", n);

		// A nap of exactly 100us
		usleep(100);
	}
}

int main(int argc, char **argv) {
	pthread_t philosopher[5];

	for(int i = 0; i < 5; i++) {
		pthread_mutex_init(&chopstick[i], NULL);
	}

	for(int i = 0; i < 5; i++) {
		pthread_create(&philosopher[i], NULL, think_eat_sleep, (void *) i);
	}

	for(int i = 0; i < 5; i++) {
		pthread_join(philosopher[i], NULL);
	}

	for(int i = 0; i < 5; i++) {
		pthread_mutex_destroy(&chopstick[i]);
	}

	return 0;
}

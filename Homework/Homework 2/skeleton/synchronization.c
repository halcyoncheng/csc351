#include "synchronization.h"

#include <stdlib.h>

#include "threads.h"

struct waiter_t {
	int thread_id;
};

int thread_mutex_init(thread_mutex_t *mutex) {
	if(mutex) {
		atomic_init(&(mutex->locked), 0);
	}

	return 0;
}

int thread_mutex_lock(thread_mutex_t *mutex) {
	while(1) {
		// An empty while loop. In production code, you would insert
		// a "pause" assembly instruction inside the loop (on Intel)
		// in order to clear speculative operations in the CPU pipeline.
		while(mutex->locked); // spin!

		int expected = 0;

		if(atomic_compare_exchange_strong(&(mutex->locked), &expected, 1)) {
			break;
		}
	}

	return 0;
}

int thread_mutex_unlock(thread_mutex_t *mutex) {
	atomic_store(&mutex->locked, 0);

	return 0;
}

int thread_cond_init(thread_cond_t *condition_variable) {
	return 0;
}

int thread_cond_wait(thread_cond_t *condition_variable, thread_mutex_t *mutex) {
	return 0;
}

int thread_cond_signal(thread_cond_t *condition_variable) {
	return 0;
}

int thread_cond_broadcast(thread_cond_t *condition_variable) {
	return 0;
}

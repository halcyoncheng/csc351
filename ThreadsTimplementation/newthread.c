#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<setjmp.h>

#include<unistd.h>
#include<signal.h>
#include<sys/time.h>

#define STACK_SIZE 65536

jmp_buf buffer;
int created = 0;

char stack[STACK_SIZE];

void threadFunction() {
	while(1) {
		puts("Ha! ");
		sleep(1);
	}
}
void sigusr_handler(int signal_number) {
	if(setjmp(buffer) == 0) {
		created = 1;
	}
	else {
		threadFunction();
	}
}

int main(int argc, char **argv) {
	// Setup signal handler

	struct sigaction sigusr_hints;

	memset(&sigusr_hints, 0, sizeof(struct sigaction));
	sigusr_hints.sa_handler = sigusr_handler;
	sigusr_hints.sa_flags = SA_ONSTACK; // <<-- Look at this
	sigemptyset(&sigusr_hints.sa_mask);

	if(sigaction(SIGUSR1, &sigusr_hints, NULL) == -1) {
		perror("sigaction/SIGUSR1");
		exit(EXIT_FAILURE);
	}

	// Send a signal to myself, and specify a new stack

	stack_t new_stack;
	stack_t old_stack;

	new_stack.ss_flags = 0;
	new_stack.ss_size = STACK_SIZE;
	new_stack.ss_sp = stack;

	if(sigaltstack(&new_stack, &old_stack) == -1) {
		perror("sigaltstack");
		exit(EXIT_FAILURE);
	}

	raise(SIGUSR1);

	while(!created) {}; // spin!

	puts("Still in the main thread... switching");
	fflush(stdout);

	longjmp(buffer, 1);

	puts("You will never see this");
	fflush(stdout);
}

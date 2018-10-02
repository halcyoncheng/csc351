#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<setjmp.h>

#include<unistd.h>
#include<signal.h>
#include<sys/time.h>

#define STACK_SIZE 65536

jmp_buf main_buffer;
jmp_buf buffer;
int created = 0;

char stack[STACK_SIZE];

void threadFunction(void) {
	int counter = 0;

	while(1) {
		puts("Ha! ");
		// Switch to mainThreadFunction every X steps
		sleep(1);
	}
}

void mainThreadFunction(void) {
	int counter = 0;

	while(1) {
		puts("Main! ");
		// Switch to mainThreadFunction every X steps
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

	mainThreadFunction();

	puts("You will never see this");
	fflush(stdout);
}

/**
 * threads.c
 * Author: Alex Cheng, Jenny Zhong
 */
#include "threads.h"
#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<string.h>
#include<sys/time.h>

#define STACK_SIZE 65536

tcb_t thread_context[MAX_THREADS];
tcb_t *current_thread_context;
char stacks[MAX_THREADS * STACK_SIZE];
struct itimerval interval;
struct itimerval stop;
int created = 0;
int current_thread;
int count = 0;
int option = 0;

void sigusr_handler(int signal_number) {
	if(setjmp(thread_context[current_thread].buffer) == 0) {
		created = 1;
	}
	else {
        if(option == 1){
            setitimer(ITIMER_REAL, &interval, NULL);
        }
		thread_context[current_thread].function(thread_context[current_thread].argument);
	}
}

void sigalrm_handler(int signum){
    thread_yield();
}

void setupSignalHandler(int signal, void (*handler)(int)) {
	struct sigaction options;
	memset(&options, 0, sizeof(struct sigaction));
	options.sa_handler = handler;
	if(sigaction(signal, &options, NULL) == -1) {
		perror("sigaction");
	exit(EXIT_FAILURE);
	}
}

void thread_init(int preemption_enabled){
    option = preemption_enabled;
    if(preemption_enabled){
        interval.it_interval.tv_sec = 0;
        interval.it_interval.tv_usec = 100000;
        interval.it_value.tv_sec = 0;
        interval.it_value.tv_usec = 100000;
        stop.it_interval.tv_sec = 0;
        stop.it_interval.tv_usec = 0;
        stop.it_value.tv_sec = 0;
        stop.it_value.tv_usec = 0;
        setupSignalHandler(SIGALRM, sigalrm_handler);
        setitimer(ITIMER_REAL, &interval, NULL);
    }
    

    for(int i = 0; i < MAX_THREADS; i++){
        thread_context[i].state = STATE_INVALID;
    }
	thread_context[0].number = 0;
    thread_context[0].function = NULL;
    thread_context[0].argument = NULL;
    thread_context[0].return_value = NULL;
    thread_context[0].state = STATE_ACTIVE;
    thread_context[0].stack = NULL;
    current_thread_context = &thread_context[0];

    struct sigaction sigusr_hints;
    memset(&sigusr_hints, 0, sizeof(struct sigaction));
	sigusr_hints.sa_handler = sigusr_handler;
	sigusr_hints.sa_flags = SA_ONSTACK; // <<-- Look at this
	sigemptyset(&sigusr_hints.sa_mask);

    if(sigaction(SIGUSR1, &sigusr_hints, NULL) == -1) {
		perror("sigaction/SIGUSR1");
		exit(EXIT_FAILURE);
	}
    
}

int thread_create(void *(*function)(void *), void *argument){
    int i = 0;
    stack_t new_stack;
    while(thread_context[i].state != STATE_INVALID && i < MAX_THREADS){
        i++;
        if(i == MAX_THREADS){
            printf("Full! Cannot create a new thread.\n");
            return EXIT_FAILURE;
        }
    }
    thread_context[i].state = STATE_ACTIVE;
    thread_context[i].function = function;
    thread_context[i].argument = argument;
    thread_context[i].stack = &stacks[(i - 1) * STACK_SIZE];
    thread_context[i].number = i;
    new_stack.ss_flags = 0;
	new_stack.ss_size = STACK_SIZE;
	new_stack.ss_sp = thread_context[i].stack;
    if(sigaltstack(&new_stack, 0) == -1) {
		perror("sigaltstack");
		exit(EXIT_FAILURE);
	}
	current_thread = i;
    raise(SIGUSR1);
    while(!created){};
    created = 0;
    current_thread = (*current_thread_context).number;
    return i;
}

int thread_yield(void){
    if(option == 1){
    setitimer(ITIMER_REAL, &stop, NULL);
    }
    int i = 0;
    while(thread_context[i].state != STATE_ACTIVE || current_thread_context == &thread_context[i]){
        i++;
        
        if(i == MAX_THREADS){
            return 0;
        }
    }
    
    if(setjmp(thread_context[current_thread].buffer) == 0){
        current_thread_context = &thread_context[i];
        current_thread = i;
        longjmp(thread_context[i].buffer, 1);
    }
    else{
        if(option == 1){
        setitimer(ITIMER_REAL, &interval, NULL);
        }
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

void thread_exit(void *return_value){
    int joiner = (*current_thread_context).joiner_thread_number;
    (*current_thread_context).state = STATE_FINISHED;
    (*current_thread_context).return_value = return_value;
    if(joiner){
        thread_context[joiner].state = STATE_ACTIVE;
        current_thread_context = &thread_context[joiner];
        current_thread = joiner;
        longjmp(thread_context[joiner].buffer, 1);
    }
    else{
        thread_yield();
    }
}

void thread_join(int target_thread_number){
    (*current_thread_context).state = STATE_BLOCKED;
    thread_context[target_thread_number].joiner_thread_number = current_thread;
    if(setjmp(thread_context[current_thread].buffer) == 0){
        current_thread_context = &thread_context[target_thread_number];
        current_thread = target_thread_number;
        longjmp(thread_context[target_thread_number].buffer, 1);
    }
    else{
        printf("Waited thread has finished.\n");
    }
}
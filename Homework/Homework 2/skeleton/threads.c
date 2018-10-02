#include "threads.h"
#include<stdio.h>
#include<stdlib.h>
#include<signal.h>
#include<string.h>

#define STACK_SIZE 65536

tcb_t thread_context[MAX_THREADS];
tcb_t *current_thread_context;
stack_t new_stack;
int created = 0;
int current_thread;
int count = 0;

void sigusr_handler(int signal_number) {
	if(setjmp(thread_context[current_thread].buffer) == 0) {
		created = 1;
	}
	else {
		thread_context[current_thread].function(thread_context[current_thread].argument);
	}
}

void thread_init(int preemption_enabled){ 
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
    char stack[STACK_SIZE];
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
    thread_context[i].stack = stack;
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
    return i;
}

int thread_yield(void){
    int i = 0;
    // count++;
    while(thread_context[i].state != STATE_ACTIVE || current_thread_context == &thread_context[i]){
        i++;
        
        if(i == MAX_THREADS){
            return 0;
        }
    }
    // printf("next:%d\n", i);
    // printf("current:%d\n", current_thread);
    if(setjmp(thread_context[current_thread].buffer) == 0){
        
        current_thread_context = &thread_context[i];
        current_thread = i;
        longjmp(thread_context[i].buffer, 1);
    }
    else{
        
        // printf("count:%d\n", count);
        thread_context[current_thread].function(thread_context[current_thread].argument);
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
    // printf("%d\n", (*current_thread_context).number);
    (*current_thread_context).state = STATE_BLOCKED;
    (*current_thread_context).joiner_thread_number = current_thread;
    if(setjmp(thread_context[current_thread].buffer) == 0){
        current_thread_context = &thread_context[target_thread_number];
        current_thread = target_thread_number;
        longjmp(thread_context[target_thread_number].buffer, 1);
    }
    else{
        thread_context[current_thread].function(thread_context[current_thread].argument);
    }
}
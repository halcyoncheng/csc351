#include "threads.h"
#include<stdio.h>
#include<stdlib.h>

char* stacks;
tcb_t thread_context[MAX_THREADS];
tcb_t *current_thread_context;
void thread_init(int preemption_enabled){
    
    stacks = (char*) malloc(MAX_THREADS * 64 * 1024);
    for(int i = 0; i < MAX_THREADS; i++){
        thread_context[i].state = STATE_INVALID;
    }
    
    if(setjmp(thread_context[0].buffer) == 0) {
		thread_context[0].number = 0;
        thread_context[0].function = NULL;
        thread_context[0].argument = NULL;
        thread_context[0].return_value = NULL;
        thread_context[0].state = STATE_ACTIVE;
        thread_context[0].stack = &stacks[0];
        current_thread_context = &thread_context[0];
	}
	else {
		thread_context[0].function(thread_context[0].argument);
	}

}

int thread_create(void *(*function)(void *), void *argument){
    int i = 0;
    while(thread_context[i].state != STATE_INVALID && i < MAX_THREADS){
        i++;
    }
    if(setjmp(thread_context[i].buffer) == 0) {
		thread_context[i].state = STATE_ACTIVE;
        thread_context[i].function = function;
        thread_context[i].argument = argument;
        thread_context[i].stack = &stacks[i * 64 * 1024];
        thread_context[i].number = i;
	}
	else {
        // printf("current:%d\n", (*current_thread_context).number);
        // printf("current:%d\n", i);
		// thread_context[(*current_thread_context).number].function(thread_context[(*current_thread_context).number].argument);
        thread_context[i].function(thread_context[i].argument);
	}
    return i;
}

int thread_yield(void){
    int i = 0;
    // printf("current:%d\n", (*current_thread_context).number);
    while(thread_context[i].state != STATE_ACTIVE || current_thread_context == &thread_context[i]){
        i++;
        
        if(i == MAX_THREADS){
            return 0;
        }
    }
    // printf("next:%d\n", i);
    current_thread_context = &thread_context[i];
    longjmp(thread_context[i].buffer, 1);
}

void thread_exit(void *return_value){
    int joiner = (*current_thread_context).joiner_thread_number;
    (*current_thread_context).state = STATE_FINISHED;
    (*current_thread_context).return_value = return_value;
    if(joiner){
        thread_context[joiner].state = STATE_ACTIVE;
        current_thread_context = &thread_context[joiner];
        longjmp(thread_context[joiner].buffer, 1);
    }
    else{
        thread_yield();
    }
}

void thread_join(int target_thread_number){
    // printf("%d\n", (*current_thread_context).number);
    (*current_thread_context).state = STATE_BLOCKED;
    current_thread_context = &thread_context[target_thread_number];
    longjmp(thread_context[target_thread_number].buffer, 1);
}
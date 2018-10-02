#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<unistd.h>
#include<signal.h>
#include<sys/time.h>

void sigalm_handler(int signal) {
	puts("Hey ");
}

int main(int argc, char **argv) {
	// Setup signal handler

	struct sigaction options;

	memset(&options, 0, sizeof(struct sigaction));
	options.sa_handler = sigalm_handler;

	if(sigaction(SIGALRM, &options, NULL) == -1) {
		perror("sigaction/SIGALRM");
		exit(EXIT_FAILURE);
	}

	// Setup periodic alarm
	struct itimerval timer;

	timer.it_interval.tv_sec = 0;
	timer.it_interval.tv_usec = 100000;

	timer.it_value.tv_sec = 0;
	timer.it_value.tv_usec = 100000;

	if(setitimer(ITIMER_REAL, &timer, NULL) == -1) {
		perror("setitimer");
		exit(EXIT_FAILURE);
	}

	while(1) { sleep(100); } // spin
}

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<signal.h>

#define BUFFER_SIZE 128

int collectedChildren = 0;
int coolFlag = 0;
int doneFlag = 0;

// Prototypes (forward declarations) for the functions below
void setupSignalHandler(int signal, void (*handler)(int));
void childHandler(int signal);
void termHandler(int signal);
void usr1Handler(int signal);

int main(int argc, char **argv) {
	int childID;

	// This is a user-defined function. Look inside of it, below.
	setupSignalHandler(SIGCHLD, childHandler);
	setupSignalHandler(SIGTERM, termHandler);

	setupSignalHandler(SIGUSR1, usr1Handler);

	while(!doneFlag) {
		if(!coolFlag) {
			printf("Number of finished children: %d\n", collectedChildren);
		}
		else {
			printf("%d kids are done and are ready for ice cream\n", collectedChildren);
		}

		childID = fork();

		if(childID == 0) {
			return EXIT_SUCCESS;
		}

		// Ask me why you need this
		if(sleep(1) != 0) {
			sleep(1);
		}
	}

	return EXIT_SUCCESS;
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

void childHandler(int signal) {
	int status;

	while(waitpid(-1, &status, WNOHANG) != -1) {
		collectedChildren++;
	}

	// At this point, ERRNO is set, but we don't care about it
}

void termHandler(int signal) {
	doneFlag = 1;
}

void usr1Handler(int signal) {
	coolFlag = 1;
}

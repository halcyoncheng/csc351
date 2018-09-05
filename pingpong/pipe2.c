#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>

#define BUFFER_SIZE 128

int main(int argc, char **argv) {
	int variable = 10;
	int childID;

	int pipefds[2];

	if(pipe(pipefds) != 0) {
		perror("pipe");

		return EXIT_FAILURE;
	}

	// At this point, pipefds[0] is for reading, pipefds[1] is for writing

	childID = fork();

	if(childID == 0) {
		// TODO: Child: Make the standard output be the writing end

		if(execlp("ls", "ls", "-l", (char *) NULL) == -1) {
			perror("execlp");
		}

		// If you're here, the above exec failed
		return EXIT_FAILURE;
	}

	childID = fork();

	if(childID == 0) {
		// TODO: Child: Make the standard input be the reading end

		if(execlp("grep", "grep", "pip", (char *) NULL) == -1) {
			perror("Error creating child process");
		}

		// If you're here, the above exec failed
		return EXIT_FAILURE;
	}

	// Parent: I'm not interested in any other process' business
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(pipefds[0]);
	close(pipefds[1]);

	// Wait until children are finished
	int returnStatus;

	wait(&returnStatus);
	wait(&returnStatus);

	return EXIT_SUCCESS;
}

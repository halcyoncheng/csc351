#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

#define BUFFER_SIZE 128

int main(int argc, char **argv) {
	int variable = 10;
	int childID;
	int pipefds[2];

	// Use pipe(2) here

	// At this point, pipefds[0] is for reading, pipefds[1] is for writing
	if(pipe(pipefds) != 0){
		perror("pipe");
		return EXIT_FAILURE;
	}

	childID = fork();

	if(childID == 0) {
		// Child: close the writing end
		// TODO
		close(pipefds[1]);


		char buffer[BUFFER_SIZE];

		int nread;

		printf("Child prints: ");
		fflush(stdout); // If you remove this, the message above can go AFTER the message

		while ((nread = read(pipefds[0], buffer, BUFFER_SIZE)) > 0) {
			write(STDOUT_FILENO, buffer, nread);
		}

		return EXIT_SUCCESS;
	}
	close(pipefds[0]);

	// Parent: close the reading end
	// TODO here

	sleep(2);

	// Send a message to the child

	char *message = "Please be home by 10pm and keep your cell phone on\n";

	// Has to copy strlen(message) + 1 bytes to include the null character in the end!!!
	// write(...)
	write(pipefds[1], message, strlen(message) + 1);

	// collect child status
	int returnStatus;

	wait(&returnStatus);

	return EXIT_SUCCESS;
}

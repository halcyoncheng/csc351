// This cat is WILD!

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

#define BUFFER_SIZE 1024

int main(int argc, char **argv) {
	char buffer[BUFFER_SIZE];

	int nread;
	int nwrite;

	while ((nread = read(STDIN_FILENO, buffer, BUFFER_SIZE)) > 0) {
		int offset = 0;

		while((nwrite = write(STDOUT_FILENO, buffer + offset, nread - offset)) > 0) {
			offset += nwrite;
		}

		if(nread < 0) {
			perror("read");
			return EXIT_FAILURE;
		}
	}

	if(nread < 0) {
		perror("read");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

#define BUFFER_SIZE 1024

int main(int argc, char **argv) {
	char buffer[BUFFER_SIZE];

	int nread;

	while ((nread = read(STDIN_FILENO, buffer, BUFFER_SIZE)) > 0) {
		write(STDOUT_FILENO, buffer, nread);
	}

	return EXIT_SUCCESS;
}

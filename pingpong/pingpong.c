#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>

#define BUFFER_SIZE 1024

#define FILENAME "newfile.txt"

int main(int argc, char **argv) {
	char buffer[BUFFER_SIZE];
	
	int fd;

	int nread;
	int nwrite;

	// CREATE THE FILE

	if((fd = open(FILENAME, O_WRONLY | O_CREAT, 0644)) == -1) {
		perror("open");
		return EXIT_FAILURE;
	}

	strncpy(buffer, "This is a short string that should be written to the file.", BUFFER_SIZE);

	// Try putting a +1 here: what happens?
	int ntowrite = strlen(buffer);
	int offset = 0;

	while((nwrite = write(fd, buffer + offset, ntowrite - offset)) > 0) {
		offset += nwrite;
	}

	close(fd);

	// READ THE CONTENTS BACK

	if((fd = open(FILENAME, O_RDONLY)) == -1) {
		perror("open");
		return EXIT_FAILURE;
	}

	while((nread = read(fd, buffer, BUFFER_SIZE)) > 0) {
		offset = 0;

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

	close(fd);

	return EXIT_SUCCESS;
}

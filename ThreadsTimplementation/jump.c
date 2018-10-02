#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<setjmp.h>

int main(int argc, char **argv) {
	jmp_buf buffer;

	if(setjmp(buffer) == 0) {
		printf("Saved the buffer, now jumping\n");
		fflush(stdout);

		longjmp(buffer, 1);

		printf("This should not be printed\n");
		fflush(stdout);
	}
	else {
		printf("I'm in the else part too\n");
		fflush(stdout);
	}
}

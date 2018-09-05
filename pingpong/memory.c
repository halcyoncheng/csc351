#include<stdio.h>
#include<stdlib.h>

// Ugh
int globalVariable;

int function(int a, int b, int c);
int function2(int a, int b, int c);

int main(int argc, char **argv) {
	char a;
	short b;
	int c;
	long int d;
	char *pointer1;
	int *pointer2;
	
	printf("sizeof(char) = %lu\n", sizeof(a));
	printf("sizeof(short) = %lu\n", sizeof(b));
	printf("sizeof(int) = %lu\n", sizeof(c));
	printf("sizeof(long int) = %lu\n", sizeof(d));
	printf("sizeof(char *) = %lu\n", sizeof(pointer1));
	printf("sizeof(int *) = %lu\n", sizeof(pointer2));

	pointer1 = (char *) malloc(30 * sizeof(char));

	pointer2 = (int *) malloc(30 * sizeof(int));

	// Here you can use pointer1[0]...pointer1[29]. Same for pointer2.

	free(pointer1);
	free(pointer2);

	c = function(20, 40, 60);
	c = function2(20, 40, 60);

	return EXIT_SUCCESS;
}

// Never give names such as "a", "b", or "c"
int function(int a, int b, int c) {
	return a * b + c;
}

// Never give names such as "a", "b", or "c"
int function2(int a, int b, int c) {
	static int var = 10;

	return (a + b + c) * (var++);
}

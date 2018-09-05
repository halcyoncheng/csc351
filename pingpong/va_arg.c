#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>

void function(char *pattern, ...) {
	char character = 'a';
	int number = 0;
	long int big_number = 0L;
	double real_number = 0.0;

	va_list arguments;
	va_start(arguments, pattern);

	for(char *c = pattern; *c != '\0'; c++) {
		switch(*c) {
		case'd':
			number = va_arg(arguments, int);
			break;
		case'l':
			big_number = va_arg(arguments, long int);
			break;
		case'f':
			// Anything that is promoted to a double (float) should be captured as double and casted
			real_number = (float) va_arg(arguments, double);
			break;
		case'c':
			// Anything that is promoted to an int (char, short int...) should be captured as int and casted
			character = (char) va_arg(arguments, int);
			break;
		}
	}

	va_end(arguments);
	printf("character = %c, number = %d, big_number = %ld, real_number = %f\n", character, number, big_number, real_number);
}

struct type {
	int a;
	int b;
};

int main(int argc, char **argv) {
	uint32_t var32bits = 3;
	uint64_t var64bits = 3;

	struct type arr[10];

	arr[0] = (struct type) { .a = 10, .b = 20 };

	function("cdlfc", 'x', 50, 20L, 30.0, 'y');
}

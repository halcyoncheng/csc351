#include<stdio.h>
#include<stdlib.h>

#include <superutil.h>

int main(int argc, char **argv) {
	// This file can compile because it can see the "visible" variable,
	// as "superutil.h" indicates that such variable exists
	visible = 0;

	printf("call functionA() = %d\n", functionA());
	printf("call functionB() = %d\n", functionB());

	return EXIT_SUCCESS;
}

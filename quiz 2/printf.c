#include<stdio.h>
#include<string.h>
#include<stdarg.h>

// You can only use strlen, putchar, and va_[list,start,arg,end]
// in this assignment. No other library functions should be necessary.

// TODO
void reverse(char *buffer) {
}

// TODO
char* itoa(long int number, char* buffer, int base) {
}

// TODO
int myputs(char *string) {
}

// TODO
int myprintf(char *format, ...) {
}

int main(int argc, char **argv) {
	myprintf("Number is %d\n", 132);
	myprintf("Negative number is %d\n", -132);

	unsigned int u = 4096;
	myprintf("Unsigned number is %u\n", u);
	myprintf("Unsigned number in hex is %x\n", u);

	myprintf("Long number is %l\n", 5000000000);
	myprintf("Long negative number is %l\n", -5000000000);
}

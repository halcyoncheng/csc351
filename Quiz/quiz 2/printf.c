/** Quiz 2
 * Author: Alex Cheng, Yilin Wang
 */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdarg.h>

// You can only use strlen, putchar, and va_[list,start,arg,end]
// in this assignment. No other library functions should be necessary.

// TODO
void reverse(char *buffer) {
	int len = strlen(buffer);
	char newstr[len + 1];
	strcpy(newstr, buffer);
	for(int i = 0; i < len; i++){
		buffer[i] = newstr[len - 1 - i];
	}
}

// TODO
char* itoa(long int number, char* buffer, int base) {
	switch(base){
		case 10:;
			if(number > 0){
				int i = 0;
				while(number != 0){
					int temp = number % 10;
					number = number / 10;
					buffer[i++] = temp + '0';
				}
				buffer[i] = '\0';
				reverse(buffer);
			}
			else{
				int i = 0;
				number = -number;
				while(number != 0){
					int temp = number % 10;
					number = number / 10;
					buffer[i++] = temp + '0';
				}
				buffer[i++] = '-';
				buffer[i] = '\0';
				reverse(buffer);
			}
			break;
		case 16:;
			if(number > 0){
				int i = 0;
				while(number != 0){
					int temp = number % 16;
					if(temp < 10){
						buffer[i++] = temp + 48;
					}
					else{
						buffer[i++] = (char) temp + 87;
					}
					number = number / 16;
				}
				buffer[i++] = 'x';
				buffer[i++] = '0';
				buffer[i] = '\0';
				reverse(buffer);
			}
			else{
				int i = 0;
				number = -number;
				while(number != 0){
					int temp = number % 16;
					if(temp < 10){
						buffer[i++] = temp + 48;
					}
					else{
						buffer[i++] = (char) temp + 87;
					}
					number = number / 16;
				}
				buffer[i++] = 'x';
				buffer[i++] = '0';
				buffer[i++] = '-';
				buffer[i] = '\0';
				reverse(buffer);
			}
			break;
		default: break;
			
	}

	return buffer;
}

// TODO
int myputs(char *string) {
	for(int i = 0; i < strlen(string); i++){
		putchar(string[i]);
	}
	return EXIT_SUCCESS;
}

// TODO
int myprintf(char *format, ...) {
	va_list ap;
	va_start(ap, format);
	int d, x;
	unsigned u;
	long int l;
	char tempd[sizeof(int) * 8 + 1];
	char tempu[sizeof(unsigned) * 8 + 1];
	char templ[sizeof(long int) * 8 + 1];
	while(*format){
		if(*format == '%'){
			*format++;
			switch(*format++){
				case 'd':;
					d = va_arg(ap, int);
					myputs(itoa(d, tempd, 10));
					break;
				case 'u':;
					u = va_arg(ap, unsigned);
					myputs(itoa(u, tempu, 10));
					break;
				case 'x':;
					x = va_arg(ap, int);
					myputs(itoa(x, tempd, 16));
					break;
				case 'l':;
					l = va_arg(ap, long int);
					myputs(itoa(l, templ, 10));
					break;
				default: break;
			}
		}
		else{
			putchar(*format++);
		}
	}
	va_end(ap);
	return 0;
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

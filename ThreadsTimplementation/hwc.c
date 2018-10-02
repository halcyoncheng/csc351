#include <unistd.h>

int main(void) {
        write(STDOUT_FILENO, "Hello World!\n", 13);
        _exit(0); // exit() is a libc wrapper to _exit()
}

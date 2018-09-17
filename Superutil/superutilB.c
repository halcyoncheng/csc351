#include "superutil.h"

// "static" here means local to the module
static int notVisible = 256;

int functionB(void) {
	return visible + notVisible;
}

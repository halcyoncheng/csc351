#include "superutil.h"

// This is visible but another module has to declare (but not instantiate) this variable
// See superutil.h
int visible = 10;

// "static" here means local to the module
static int notVisible = 42;

int functionA(void) {
	return visible + notVisible;
}

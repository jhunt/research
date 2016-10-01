#define ASSERTION_OUTPUT_STREAM  stdout
#define ASSERTION_FAIL_EXIT_CODE 23
#define ASSERTION_DEBUGGING
#define ASSERTION_PRINT_ALWAYS
#include "insist.h"
#include <stdio.h>

int main(int argc, char **argv) {
	int x = 421;
	insist(x == 42, "x should be the answer to life, the universe, and everything");
	return 0;
}

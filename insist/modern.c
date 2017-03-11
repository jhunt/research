#define ASSERTION_DEBUGGING
#include "insist.h"
#include <stdio.h>

int _strlen(const char *s)
{
	insist(s != NULL, "_strlen(NULL) is undefined");

	int n = 0;
	while (*s++)
		n++;

	return n;
}

void thunk() {
	_strlen(NULL); /* should fail */
}

int main(int argc, char **argv)
{
	thunk();
	thunk();
	thunk();
	return 0;
}

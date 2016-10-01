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

int main(int argc, char **argv)
{
	_strlen(NULL); /* should fail */
	return 0;
}

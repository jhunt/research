#include <assert.h>
#include <stdio.h>

int _strlen(const char *s)
{
	assert(s != NULL);

	int n = 0;
	while (*s++)
		n++;

	return n;
}

int main(int argc, char **argv)
{
	fprintf(stderr, "before the assertion\n");
	_strlen(NULL); /* should fail */
	fprintf(stderr, "after the assertion\n");
	return 0;
}

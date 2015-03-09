#include <stdio.h>
#include <stdlib.h>

int main (int argc, char **argv)
{
	if (malloc(24) == NULL)
		fprintf(stderr, "first malloc failed\n");
	if (malloc(24) == NULL)
		fprintf(stderr, "second malloc failed\n");
	if (malloc(24) == NULL)
		fprintf(stderr, "third malloc failed\n");

	if (calloc(48, sizeof(char)) == NULL)
		fprintf(stderr, "first calloc failed\n");
	if (calloc(48, sizeof(char)) == NULL)
		fprintf(stderr, "second calloc failed\n");
	if (calloc(48, sizeof(char)) == NULL)
		fprintf(stderr, "third calloc failed\n");
	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
	char *bin = strrchr(argv[0], '/');
	if (!bin) bin = argv[0];
	else bin++;

	if (strcmp(bin, "wastrel") == 0) {
		fprintf(stderr, "wastrel\n");
		int n = (argc < 2 ? 1024 : atoi(argv[1])) * 1024;
		char **m = calloc(n / 64, sizeof(char **));

		fprintf(stderr, "allocating %u 64k blocks (%uM)\n", n / 64, n / 1024);
		int i;
		for (i = 0; i < n / 64; i++) {
			m[i] = calloc(64 * 1024, sizeof(char));
			memset(m, rand(), 64 * 1024);
		}

		// FIXME: can we just use a single sbrk() call to extend the heap?

	} else {
		fprintf(stderr, "unknown tool '%s'\n", bin);
		return 1;
	}
	return 0;
}

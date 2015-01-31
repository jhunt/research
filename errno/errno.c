#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "elist.h"

int main (int argc, char **argv)
{
	elist_init();
	int i;
	for (i = 1; argv[i]; i++) {
		char *x = NULL;
		int err = strtoul(argv[i], &x, 10);
		if (x && *x) {
			fprintf(stderr, "'%s': unrecognized errno value\n", argv[i]);
			continue;
		}
		if (err < 0 || err > 255) {
			fprintf(stderr, "%i: out-of-range for an errno value\n", err);
			continue;
		}

		fprintf(stderr, "% 4i %-16s   %s\n",
			err, CONST[err] ? CONST[err] : "-", strerror(err));
	}
	return 0;
}

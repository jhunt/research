#include <stdio.h>
#include <stdlib.h>

#define FLAG_A 1
#define FLAG_B 2
#define FLAG_C 4

int main(int argc, char **argv)
{
	int i;

	srand(99); /* need arbitrary numbers */
	for (i = 0; i < 40; i++) {
		int v = (int)(rand() * 1.0 / RAND_MAX * 8);

		fprintf(stderr, "% 2d: ", v);
		/* this doesn't work:
		switch (1) {
		case v & FLAG_A: fprintf(stderr, "A "); break;
		case v & FLAG_B: fprintf(stderr, "B "); break;
		case v & FLAG_C: fprintf(stderr, "C "); break;
		}
		*/
		/* but this does: */
		if (v & FLAG_A) fprintf(stderr, "A ");
		if (v & FLAG_B) fprintf(stderr, "B ");
		if (v & FLAG_C) fprintf(stderr, "C ");
		fprintf(stderr, "\n");
	}
	return 0;
}

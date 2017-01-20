#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define F(x) exp(-1*x*x/2)

#define V_FACTOR 1.0e-5

/* integrates e^(-x^2/2) from [3.6..inf) */
long double find_v(long double r, long double ep) {
	long double x, y, acc;

	acc = r * F(r);
	x = r;
	do {
		y = F(x);
		acc += y * V_FACTOR;
		x += V_FACTOR;
	} while (y > ep);

	return acc;
}

int main(int argc, char **argv) {
	long double r, ep;
	char *end;

	if (argc != 3) {
		fprintf(stderr, "USAGE: %s R EP\n", argv[0]);
		return 1;
	}

	/* arg 1 - value for r */
	r = strtod(argv[1], &end);
	if (end != NULL && *end != '\0') {
		fprintf(stderr, "'%s' doesn't look like a floating-point number... (starting at '%s')\n", argv[1], end);
		return 1;
	}
	if (r <= 0) {
		fprintf(stderr, "%20.18Le is not a valid r (must be positive, non-zero)\n", r);
		return 1;
	}

	/* arg 2 - epsilon for termination */
	ep = strtod(argv[2], &end);
	if (end != NULL && *end != '\0') {
		fprintf(stderr, "'%s' doesn't look like a floating-point number... (starting at '%s')\n", argv[2], end);
		return 1;
	}
	if (r <= 0) {
		fprintf(stderr, "%20.18Le is not a valid ep (must be positive, non-zero)\n", ep);
		return 1;
	}

	printf("integral of y = e^(-x2/2) from [%Lf, inf), given epsilon %Le, is %20.18Le\n",
		r, ep, find_v(r, ep));
	return 0;
}

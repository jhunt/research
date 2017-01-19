#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define F(x)  (long double)exp(-1 * (x) * (x) / 2)
#define F_(y) (long double)sqrt(-2.0 * log(y))

#define R_FACTOR 1.0e-3
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

long double find_x0(long double r, int n)
{
	long double x, v;
	int i;

	x = r;
	v = find_v(r, 0.001);
	for (i = n - 2; i >= 0; i--) {
		if (i == 1 || i == n - 1)
		fprintf(stderr, "x%-3d = F_(%20.18Le / %20.18Le + F(%20.18Le)) = %20.18Le\n",
			i, v, x, x, F_(v/x + F(x)));
		x = F_(v/x + F(x));
	}
	return (v - x + x * F(x));
}

int main(int argc, char **argv) {
	int i, nboxes;
	long double r, x, last, d;
	char *end = NULL;

	if (argc != 3) {
		fprintf(stderr, "USAGE: %s NBOXES R\n", argv[0]);
		return 1;
	}

	/* arg 1 - number of boxes */
	nboxes = strtol(argv[1], &end, 10);
	if (end != NULL && *end != '\0') {
		fprintf(stderr, "'%s' doesn't look like an integer... (starting at '%s')\n", argv[1], end);
		return 1;
	}
	if (nboxes <= 0) {
		fprintf(stderr, "%d is not a valid number of boxes (must be positive, non-zero)\n", nboxes);
		return 1;
	}

	/* arg 2 - guess for r */
	r = strtod(argv[2], &end);
	if (end != NULL && *end != '\0') {
		fprintf(stderr, "'%s' doesn't look like a floating-point number... (starting at '%s')\n", argv[2], end);
		return 1;
	}
	if (r <= 0) {
		fprintf(stderr, "%20.18Le is not a valid r (must be positive, non-zero)\n", r);
		return 1;
	}

	/* bisect!
	   first, increment r by d until we don't get a NaN;
	     then, decrement r by d until we hit a NaN;
	     then, make d smaller by one order of magnitude and try again
	   algorithm terminates when z(r) == 0.0 (not likely)
	     or when r + d == r (i.e., d is too small to meaningfully affect
	                         the value of r, and continued calculation
	                         will just lead to an infinite loop)
	 */
	d = 1.0e-2;
	for (i = 0; i < 10; i++) {
		if (r + d == r) {
			fprintf(stderr, "z(r) is close to 0 for r = %20.18Le\n", r);
			return 1;
		}
		for (x = find_x0(r, nboxes); isnan(x); r += d, x = find_x0(r, nboxes));
		if (x == 0) {
			fprintf(stderr, "z(r) = 0 for r = %20.18Le\n", r);
			return 0;
		}
		d *= -0.1;
		for (x = find_x0(r, nboxes); !isnan(x) && x != 0.0; r += d, x = find_x0(r, nboxes));
		if (x == 0) {
			fprintf(stderr, "z(r) = 0 for r = %20.18Le\n", r);
			return 0;
		}
		d *= -0.1;
	}

	fprintf(stderr, "did not find a valid value of r for z(r) = 0, before we gave up.\n");
	return 1;
}

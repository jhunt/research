#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define F(x)  (long double)exp(-1 * (x) * (x) / 2)
#define F_(y) (long double)sqrt(-2.0 * log(y))

#define V_FACTOR 1.0e-5
#define V_EPSILON 1.0e-18

/* integrates e^(-x^2/2) from [3.6..inf) */
long double V(long double r, long double ep) {
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

long double Z(long double x, int n, long double ep, long double *v)
{
	long double t;
	int i;
	*v = V(x, ep);
	for (i = n - 2; i > 0; i--) {
		t = F_(*v/x + F(x));
		if (isnan(t)) break;
		x = t;
	}
	return (*v-x + x*F(x));
}

long double solve(long double a, long double b, long double ep, int n, long double *v)
{
	long double r, z, _;
	for (;;) {
		r = (a + b) / 2.0;
		z = Z(r, n, ep, v);
		if (b - r < ep || fabsl(z) < ep)
			return r;

		/* if we crossed the root (change in sign) back up the bisection to [a,r].
		   otherwise, the root lies "ahead" of us, at [r,b] */
		if (Z(a,n,ep,&_) * z < 0) b = r;
		else                      a = r;
	}
}

int main(int argc, char **argv) {
	int n;
	long double r, v;
	char *end = NULL;

	if (argc > 2) {
		fprintf(stderr, "USAGE: %s [N]\n(where N defaults to 256)\n", argv[0]);
		return 1;
	}

	n = 256;
	if (argc == 2) {
		n = strtol(argv[1], &end, 10);
		if (end != NULL && *end != '\0') {
			fprintf(stderr, "'%s' doesn't look like an integer... (starting at '%s')\n", argv[1], end);
			return 1;
		}
		if (n <= 0) {
			fprintf(stderr, "%d is not a valid number of boxes (must be positive, non-zero)\n", n);
			return 1;
		}
	}

	r = solve(0.5, 10, 1e-15, n, &v);
	printf("z(r) ≅ 0 for n = %i, r = %20.18Le, v = %20.18Le\n", n, r, v);
	return 0;
}

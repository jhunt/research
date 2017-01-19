#include <stdio.h>
#include <math.h>

#define F(x) exp(-1*x*x/2)

/* integrates e^(-x^2/2) from [3.6..inf) */
int main() {
	double x, y, d, ep, acc;
	acc = 0;
	d = 1.0e-5;
	ep = 1.0e-4;
	x = 3.6;
	fprintf(stderr, "f(3.6) = %le\n", F(x));
	do {
		y = F(x) * d;
		acc += y;
		x += d;
	} while (x <= 5);

	printf("integr: %le\n", acc);
	return 0;
}

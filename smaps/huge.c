#include "lib.c"

int main(int argc, char **argv)
{
	char *under = malloc(96 KB);
	randomize(under, 96 KB);

	char *over = malloc(256 KB);
	randomize(over, 256 KB);

	return interlude();
}

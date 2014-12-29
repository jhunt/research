#include "lib.c"

int main (int argc, char **argv)
{
	char buf[28 KB] = {0};
	randomize(buf, 28 KB);
	return interlude();
}

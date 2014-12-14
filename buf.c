#include "lib.c"

static char buf1[8  MB] = {0};
static char buf2[16 MB] = {0};

int main(int argc, char **argv)
{
	randomize(buf2, 16 MB);
	return interlude();
}

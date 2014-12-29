#include "lib.c"

int main(int argc, char **argv)
{
	dirty(16 MB);
	clean(32 MB);
	return interlude();
}

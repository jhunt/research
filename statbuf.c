#include "lib.c"

/*
   statbuf - static memory allocation

   This program uses a large (32M) static buffer,
   so that it gets included in the data section of
   the executable.

   While running, there should be a sizable chunk
   of Private_Dirty memory in the smaps output
   (because we overwrite the entire buffer).

 */

static char buf[32 MB] = {0};
int main(int argc, char **argv)
{
	randomize(buf, 32 MB);
	return interlude();
}

#include "lib.c"
#include <string.h>

/*
   statbuf - static memory allocation

   This program uses a large (32M) static buffer,
   so that it gets included in the data section of
   the executable.

   While running, there should be a sizable chunk
   of Private_Dirty memory in the smaps output
   (because we overwrite the entire buffer).

 */

#define BUFSIZE 32 * 1024 * 1024
static char buf[BUFSIZE] = {0};

int main(int argc, char **argv)
{
	/* this is mostly to quell GCC warnings
	   about unused (but defined) variables. */
	int c, i;
	for (i = 0; i < BUFSIZE; i++)
		c += buf[i];
	i = c;

	return wrapup();
}

#include "lib.c"
#include <string.h>

/*
   recur - stack recrusion allocation

   This program uses a moderate (48K) stack-allocated
   buffer in the activation frame of several recursive
   function calls (to grow the stack)

   While running, there should be a sizable chunk
   of Private_Dirty memory in the smaps output
   (because we overwrite the entire buffer).

 */

#define BUFSIZE 1024

static int recurse(int n)
{
	char buf[BUFSIZE] = {0};
	memset(buf, 42, BUFSIZE);

	return n == 0 ? wrapup()
	              : recurse(n - 1);
}

int main(int argc, char **argv)
{
	return recurse(256);
}

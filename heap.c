#include "lib.c"

/*
   heap - heap allocation

   This program uses a large (256M) heap-allocated
   buffer, which will not be in the binary image.

   While running, there should be a sizable chunks
   of Private_Clean and Private_Dirty  memory in the
   smaps output (because we overwrite half the buffer).

   (in reality, the 256M single-malloc causes libc
    to switch into mmap() mode to honor the request,
    so this memory will show up as an anonymous map)

 */

#define BUFSIZE 256 * 1024 * 1024

int main(int argc, char **argv)
{
	char *buf = malloc(BUFSIZE);
	randomize(buf, BUFSIZE);

	return wrapup();
}

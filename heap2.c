#include "lib.c"
#include <string.h>
#include <stdlib.h>

/*
   heap - heap allocation

   This program uses lots of small (64k) buffers, all
   allocated on the heap, to get by the MMAP_THRESHOLD
   that causes libc to resort to mmap().

   While running, there should be a sizable chunks
   of Private_Clean and Private_Dirty  memory in the
   smaps output (because we overwrite half the buffers).

 */

#define NBUFS 4096
#define BUFSIZE 64 * 1024
/* BUFSIZE * NBUFS == 256M */

int main(int argc, char **argv)
{
	char *bufs[NBUFS] = {0};
	int i;
	for (i = 0; i < NBUFS; i++) {
		bufs[i] = malloc(BUFSIZE);
		if (i < NBUFS / 2) memset(bufs[i], 42, BUFSIZE);
	}

	return wrapup();
}

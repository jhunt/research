#include "lib.c"

/*
   heap3 - heap allocation

   This program uses lots of small (64k) buffers, all
   allocated on the heap, to get by the MMAP_THRESHOLD
   that causes libc to resort to mmap().

   Since it does not overwrite any of the data it
   receives, it should have no Private_Dirty memory.

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
		use(bufs[i], BUFSIZE);
	}

	return wrapup();
}

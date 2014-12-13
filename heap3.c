#include "lib.c"

/*
   heap3 - heap allocation

   This program uses lots of small (64k) buffers, all
   allocated on the heap, to get by the MMAP_THRESHOLD
   that causes libc to resort to mmap().

   Since it does not overwrite any of the data it
   receives, it should have no Private_Dirty memory.

 */

int main(int argc, char **argv)
{
	clean(256, 1024);
	dirty(512, 1024);

	return interlude();
}

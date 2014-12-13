#include "lib.c"

#define MAPSIZE 96 * 1024 * 1024

int main(int argc, char **argv)
{
	char *buf = mmap(NULL, MAPSIZE, PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
	if (!buf) perror("mmap");

	return wrapup();
}

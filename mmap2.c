#include "lib.c"

int main(int argc, char **argv)
{
	char *buf = mmap(NULL, 64 MB, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_PRIVATE, -1, 0);
	if (!buf) perror("mmap");
	randomize(buf, 64 MB);

	char *shared = mmap(NULL, 128 MB, PROT_READ|PROT_WRITE, MAP_ANONYMOUS|MAP_SHARED, -1, 0);
	if (!shared) perror("mmap");
	randomize(shared, 128 MB);

	return wrapup();
}

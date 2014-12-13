#include "lib.c"

int main(int argc, char **argv)
{
	int fd = open("data/256k", O_RDWR);
	if (fd < 0) perror("open");

	char *file = mmap(NULL, 256 KB, PROT_READ|PROT_WRITE, MAP_PRIVATE, fd, 0);
	if (!file) perror("mmap");
	randomize(file, 256 KB);

	return wrapup();
}

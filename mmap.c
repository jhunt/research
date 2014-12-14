#include "lib.c"

int main(int argc, char **argv)
{
	/* inert map (never modified) */
	char *inert = mmap(NULL, 32 KB,
		PROT_READ|PROT_WRITE,
		MAP_ANONYMOUS|MAP_PRIVATE,
		-1, 0);

	/* anonymous, private mmap */
	char *anon_priv = mmap(NULL, 64 KB,
		PROT_READ|PROT_WRITE,
		MAP_ANONYMOUS|MAP_PRIVATE,
		-1, 0);
	randomize(anon_priv, 64 KB);

	/* anonymous, shared map */
	char *anon_shared = mmap(NULL, 128 KB,
		PROT_READ|PROT_WRITE,
		MAP_ANONYMOUS|MAP_SHARED,
		-1, 0);
	randomize(anon_shared, 128 KB);

	/* private, file-backed map */
	int fd = open("data/256k", O_RDWR);
	assert(fd >= 0);
	char *file = mmap(NULL, 256 KB,
		PROT_READ|PROT_WRITE,
		MAP_PRIVATE,
		fd, 0);
	randomize(file, 256 KB);

	return interlude();
}

#include <stdio.h>

unsigned long
hash(unsigned char *str)
{
	unsigned long hash = 5381;
	int c;

	while ((c = *str++))
		hash = ((hash << 5) + hash) + c;

	return hash;
}

int main(int argc, char **argv)
{
	int i;
	for (i = 1; i < argc; i++)
		printf("    \"%s\" = %lu\n", argv[i], hash((unsigned char*)argv[i]) & 0xff);
	printf("\n\n");
	for (i = 1; i < argc; i++)
		printf("    L(%s) = %lu mod %i = %lu\n", argv[i],
			 hash((unsigned char*)argv[i]) & 0xff, 3,
			(hash((unsigned char*)argv[i]) & 0xff) % 3);
	printf("\n\n");
	for (i = 1; i < argc; i++)
		printf("    L(%s) = %lu mod %i = %lu\n", argv[i],
			 hash((unsigned char*)argv[i]) & 0xff, 4,
			(hash((unsigned char*)argv[i]) & 0xff) % 4);
	printf("\n\n");
	return 0;
}

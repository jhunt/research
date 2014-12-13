#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#define HELPER __attribute__ ((unused))

HELPER
static void randomize(char *buf, size_t n)
{
	char c = rand() & 0xff;
	memset(buf, c, n);
}

HELPER
static void use(char *buf, size_t n)
{
	int x = 0;
	size_t i;
	for (i = 0; i < n; i++) {
		x += i;
	}
}

HELPER
static int wrapup(void)
{
	pid_t pid = getpid();
	printf("pid %i\n", (int)pid);
	printf("------------------------------------------\n"
	       "go check /proc/%i/smaps; I'll wait...\n"
	       "press <Enter> when you're done\n", pid);
	fgetc(stdin);
	return 0;
}

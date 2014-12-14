#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>
#include <fcntl.h>
#include <assert.h>

#define KB * 1024
#define MB * 1024 * 1024
#define HELPER __attribute__ ((unused))

HELPER
static void randomize(char *buf, size_t n)
{
	assert(buf);
	memset(buf, rand() & 0xff, n);
}

HELPER
static void clean(int b)
{
	for (; b > 0; b -= 1 KB)
		calloc(1 KB, sizeof(char));
}

HELPER
static void dirty(int b)
{
	for (; b > 0; b -= 1 KB)
		randomize(calloc(1 KB, sizeof(char)), 1 KB);
}

HELPER
static int interlude(void)
{
	pid_t pid = getpid();
	printf("pid %i\n", (int)pid);
	printf("------------------------------------------\n"
	       "go check /proc/%i/smaps; I'll wait...\n"
	       "press <Enter> when you're done\n", pid);
	fgetc(stdin);
	return 0;
}

void child(void)
{
	printf("child %i\n", (int)getpid());
	for (;;) sleep(15);
	exit(0);
}

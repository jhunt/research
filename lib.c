#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <fcntl.h>
#include <assert.h>

#define KB * 1024
#define MB * 1024 * 1024

static void randomize(char *buf, size_t n)
{
	assert(buf);
	memset(buf, rand() & 0xff, n);
}

static void clean(int b)
{
	for (; b > 0; b -= 1 KB)
		calloc(1 KB, sizeof(char));
}

static void dirty(int b)
{
	for (; b > 0; b -= 1 KB)
		randomize(calloc(1 KB, sizeof(char)), 1 KB);
}

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

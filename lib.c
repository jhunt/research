#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void randomize(char *buf, size_t n)
{
	char c = rand() & 0xff;
	memset(buf, c, n);
}

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

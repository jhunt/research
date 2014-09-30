#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define BUFSIZE 32 * 1024 * 1024
static char buf[BUFSIZE] = {0};

int main(int argc, char **argv)
{
	int c, i;
	for (i = 0; i < BUFSIZE; i++) {
		c += buf[i];
	}
	i = c;

	pid_t pid = getpid();
	printf("pid %i\n", (int)pid);
	printf("------------------------------------------\n"
	       "go check /proc/%i/smaps; I'll wait...\n"
	       "press <Enter> when you're done\n", pid);
	fgetc(stdin);
	return 0;
}

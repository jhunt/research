#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	pid_t pid = getpid();
	printf("pid %i\n", (int)pid);
	printf("------------------------------------------\n"
	       "go check /proc/%i/smaps; I'll wait...\n"
	       "press <Enter> when you're done\n", pid);
	fgetc(stdin);
	return 0;
}

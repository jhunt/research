#include <sys/types.h>
#include <signal.h>
#include "lib.c"

#define BUFSIZE 32 * 1024

int main(int argc, char **argv)
{
	dirty(64, 1024);
	interlude();

	pid_t pid = fork();
	if (pid <  0) perror("fork");
	if (pid == 0) {
		dirty(128, 1024);

		for (;;) sleep(15);
		exit(0);
	}

	printf("child %i\n", (int)pid);
	interlude();

	kill(pid, SIGTERM);
	return 0;
}

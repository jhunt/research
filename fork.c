#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

int main (int argc, char **argv)
{
	if (argc == 1) {
		fprintf(stderr, "USAGE: %s command --to --run -forked\n", argv[0]);
		return 1;
	}

	pid_t pid = fork();
	if (pid < 0) {
		fprintf(stderr, "fork() failed: %s\n", strerror(errno));
		return 1;
	}

	if (pid > 0) return 0;
	execvp(argv[1], argv + 1);
	fprintf(stderr, "exec() failed: %s\n", strerror(errno));
	return 1;
}

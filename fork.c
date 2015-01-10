#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main (int argc, char **argv)
{
	if (argc == 1) {
		fprintf(stderr, "USAGE: %s command --to --run -forked\n", argv[0]);
		return 1;
	}

	pid_t pid = fork();
	if (pid < 0) {
		perror("fork() failed");
		return 1;
	}

	if (pid > 0) return 0;

	if (!freopen("/dev/null", "r", stdin))  perror("</dev/null");
	if (!freopen("/dev/null", "w", stdout)) perror(">/dev/null");
	if (!freopen("/dev/null", "w", stderr)) perror("2>/dev/null");

	execvp(argv[1], argv + 1);
	perror("exec() failed");
	return 1;
}

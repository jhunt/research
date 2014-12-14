#include "lib.c"

int main(int argc, char **argv)
{
	char *buf = malloc(64 KB);
	randomize(buf, 64 KB);
	interlude();

	pid_t pid = fork();
	assert(pid >= 0);
	if (pid == 0) {
		randomize(buf, 16 KB);
		return interlude();
	}

	int st;
	waitpid(pid, &st, 0);
	return st;
}

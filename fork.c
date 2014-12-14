#include "lib.c"

int main(int argc, char **argv)
{
	dirty(64 KB);
	interlude();

	pid_t pid = fork();
	assert(pid >= 0);
	if (pid == 0) {
		dirty(128 KB);
		child();
	}

	interlude();
	kill(pid, SIGTERM);
	return 0;
}

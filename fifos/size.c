#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
	int pfd[2];
	if (pipe(pfd) != 0) {
		perror("pipe()");
		return 1;
	}

	printf("pipe buffer is %i bytes\n", fcntl(pfd[0], F_GETPIPE_SZ));
	return 0;
}

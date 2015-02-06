#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <poll.h>
#include <fcntl.h>

#define MAX_LEN 2048

static int total;
static int i = 0;

void exec_command(const char *cmd)
{
	i++; total += strlen(cmd);

	fprintf(stderr, ".");
	if (cmd[strlen(cmd) - 1] != '\n' || cmd[0] != '[')
		fprintf(stderr, "<< (%i) read %i bytes (%i total): '%s'>>", i, (int)strlen(cmd), total, cmd);
}

void process_command_file(FILE *io)
{
	char buf[MAX_LEN];
	int fd = fileno(io);

	struct pollfd pfd;

	for (;;) {
		pfd.fd = fd;
		pfd.events = POLLIN;

		int pollval = poll(&pfd, 1, 500);
		assert(pollval != -1);

		if (pollval == 0)
			continue;

		//clearerr(io);
		while (fgets(buf, MAX_LEN, io) != NULL)
			exec_command(buf);
		fprintf(stderr, "\nEOF!\n");
	}
}

int main(int argc, char **argv)
{
	unlink("io.fifo");
	system("mkfifo io.fifo");

	int fd = open("io.fifo", O_RDWR | O_NONBLOCK);
	assert(fd >= 0);

	FILE *fp = fdopen(fd, "r");
	assert(fp != NULL);

	process_command_file(fp);

	close(fd);
	fclose(fp);
	return 0;
}

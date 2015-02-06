#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <poll.h>
#include <fcntl.h>
#include <errno.h>

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

void process_command_file(int fd)
{
	char command[MAX_LEN];
	static char buf[MAX_LEN];
	static size_t offset = 0;

	struct pollfd pfd;

	for (;;) {
		pfd.fd = fd;
		pfd.events = POLLIN;

		int pollval = poll(&pfd, 1, 500);
		assert(pollval != -1);

		if (pollval == 0)
			continue;

		ssize_t n = read(fd, buf + offset, MAX_LEN - offset - 1);
		if (n == 0) {
			fprintf(stderr, "EOF!\n");
			break;
		}
		if (n == -1) {
			if (errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
				continue;
			perror("read");
			break;
		}
		buf[offset + n] = '\0';

		char *a;
		for (;;) {
			a = buf;
			while (*a && *a != '\n') a++;
			if (!*a) break;
			a++;

			memset(command, 0, MAX_LEN);
			memcpy(command, buf, a - buf);
			memmove(buf, a, MAX_LEN - (a - buf));

			exec_command(command);
		}
		offset = a - buf;
	}
	fprintf(stderr, "\nEOF!\n");
}

int main(int argc, char **argv)
{
	unlink("io.fifo");
	system("mkfifo io.fifo");

	int fd = open("io.fifo", O_RDWR | O_NONBLOCK);
	assert(fd >= 0);

	process_command_file(fd);
	fprintf(stderr, "\n\nprocessed %i items / %i bytes\n", i, total);

	close(fd);
	return 0;
}

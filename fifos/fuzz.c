#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char **argv)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);

	char buf[4096] = {0};
	if (snprintf(buf, 4095, "[%i] SCHEDULE_SERVICE_CHECK;host;example.com;foo;%i\n",
			(int)tv.tv_sec, (int)tv.tv_sec) < 0) {
		perror("snprintf");
		return 1;
	}

	size_t total;
	size_t n;
	int i, j;
	for (j = 0; j < 10; j++) {
		int fd = open("io.fifo", O_RDWR);
		if (fd < 0) {
			perror("open");
			return 1;
		}

		for (i = 0; i < 963; i++) {
			n = write(fd, buf, strlen(buf));
			total += n;
			fprintf(stderr, "%i:%i) wrote %i/%i bytes; %i total\n", j, i, (int)n, (int)strlen(buf), (int)total);
		}

		close(fd);
	}

	return 0;
}

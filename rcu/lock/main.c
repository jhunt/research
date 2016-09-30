#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include "../api.h"

/*

   rcu/lock

   This is a naïve implementation of concurrent data sharing
   across multiple threads that utilized mutex locks to ensure
   that threads see a consistent set of data.

   If my understanding of RCU read-side primitive overhead is
   correct, this program should execute the slowest, given the
   same starting parameters (number of threads, write cycles,
   and reads per write cycle).

 */

pthread_mutex_t     LOCK;
unsigned long long *COUNTER;

void writer(int cycles)
{
	while (cycles--) {
		lock(&LOCK);
		*COUNTER += 1;
		fprintf(stderr, "[writer] incremented COUNTER to %llu\n", *COUNTER);
		unlock(&LOCK);
	}
}

void* reader(void *_)
{
	int id = *(int *)_;

	for (;;) {
		lock(&LOCK);
		fprintf(stderr, "[t%d] read COUNTER at %llu\n", id, *COUNTER);
		unlock(&LOCK);
	}

	return _;
}

int main(int argc, char **argv)
{
	struct timeval start, end;
	pthread_t tid;
	int rc, i, nthreads, cycles, *ids;
	void *status;

	if (argc != 3) {
		fprintf(stderr, "USAGE: %s THREADS CYCLES READS\n", argv[0]);
		exit(2);
	}

	nthreads = atoi(argv[1]);
	cycles   = atoi(argv[2]);
	if (nthreads < 1) {
		fprintf(stderr, "Invalid THREADS value %s (must be > 0)\n", argv[1]);
		fprintf(stderr, "USAGE: %s THREADS CYCLES READS\n", argv[0]);
		exit(2);
	}
	if (cycles < 1) {
		fprintf(stderr, "Invalid WRITE_CYCLES value %s (must be > 0)\n", argv[2]);
		fprintf(stderr, "USAGE: %s THREADS CYCLES READS\n", argv[0]);
		exit(2);
	}

	make_lock(&LOCK);
	ids = xcalloc(nthreads, sizeof(int));
	COUNTER = xmalloc(sizeof(unsigned long long));

	for (i = 0; i < nthreads; i++) {
		pthread_t tid;
		ids[i] = i+1;

		rc = pthread_create(&tid, NULL, reader, &ids[i]);
		if (rc != 0) {
			perror("pthread_create");
			exit(3);
		}
	}

	gettimeofday(&start, NULL);
	writer(cycles);
	gettimeofday(&end, NULL);

	if (end.tv_usec < start.tv_usec) {
		end.tv_sec--;
		end.tv_usec += 1000000;
	}

	fprintf(stdout, "%i %li\n", nthreads,
	        ((end.tv_sec - start.tv_sec) * 1000000) + (end.tv_usec - start.tv_usec));

	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

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

struct params {
	int id;                /* used for debugging / diagnostics         */
	int write_cycles;      /* how many times to loop                   */
	int reads_per_write;   /* how many reads to perform each iteration */

	pthread_t tid;         /* stored for pthread_join use later        */
};

void* worker(void *_params)
{
	struct params *params = (struct params*)_params;
	int i, j;

	for (i = 0; i < params->write_cycles; i++) {
		for (j = 0; j < params->reads_per_write; j++) {
			if (pthread_mutex_lock(&LOCK)) {
				perror("pthread_mutex_lock");
				exit(3);
			}

			fprintf(stderr, "[t%d] read COUNTER at %llu\n", params->id, *COUNTER);

			if (pthread_mutex_unlock(&LOCK)) {
				perror("pthread_mutex_unlock");
				exit(3);
			}
		}

		if (pthread_mutex_lock(&LOCK)) {
			perror("pthread_mutex_lock");
			exit(3);
		}

		*COUNTER += 1;
		fprintf(stderr, "[t%d] incremented COUNTER to %llu\n", params->id, *COUNTER);

		if (pthread_mutex_unlock(&LOCK)) {
			perror("pthread_mutex_unlock");
			exit(3);
		}
	}

	return _params;
}

int main(int argc, char **argv)
{
	struct timeval start, end;
	pthread_t tid;
	int rc, i, nthreads, write_cycles, reads_per_write;
	void *status;
	struct params *params;

	if (argc != 4) {
		fprintf(stderr, "USAGE: %s THREADS CYCLES READS\n", argv[0]);
		exit(2);
	}

	nthreads        = atoi(argv[1]);
	write_cycles    = atoi(argv[2]);
	reads_per_write = atoi(argv[3]);
	if (nthreads < 1) {
		fprintf(stderr, "Invalid THREADS value %s (must be > 0)\n", argv[1]);
		fprintf(stderr, "USAGE: %s THREADS CYCLES READS\n", argv[0]);
		exit(2);
	}
	if (write_cycles < 1) {
		fprintf(stderr, "Invalid WRITE_CYCLES value %s (must be > 0)\n", argv[2]);
		fprintf(stderr, "USAGE: %s THREADS CYCLES READS\n", argv[0]);
		exit(2);
	}
	if (reads_per_write < 1) {
		fprintf(stderr, "Invalid READS value %s (must be > 0)\n", argv[3]);
		fprintf(stderr, "USAGE: %s THREADS CYCLES READS\n", argv[0]);
		exit(2);
	}
	params = calloc(nthreads, sizeof(struct params));
	if (!params) {
		perror("calloc");
		exit(3);
	}

	rc = pthread_mutex_init(&LOCK, NULL);
	if (rc != 0) {
		perror("pthread_mutex_init");
		exit(3);
	}

	COUNTER = calloc(1, sizeof(unsigned long long));

	gettimeofday(&start, NULL);
	for (i = 0; i < nthreads; i++) {
		params[i].id              = i+1;
		params[i].write_cycles    = write_cycles;
		params[i].reads_per_write = reads_per_write;

		rc = pthread_create(&params[i].tid, NULL, worker, &params[i]);
		if (rc != 0) {
			perror("pthread_create");
			exit(3);
		}
	}
	for (i = 0; i < nthreads; i++) {
		rc = pthread_join(params[i].tid, &status);
		if (rc != 0) {
			perror("pthread_join");
			exit(3);
		}
	}
	gettimeofday(&end, NULL);

	if (end.tv_usec < start.tv_usec) {
		end.tv_sec--;
		end.tv_usec += 1000000;
	}

	fprintf(stdout, "%i %li\n", nthreads,
	        ((end.tv_sec - start.tv_sec) * 1000000) + (end.tv_usec - start.tv_usec));
#if 0
	fprintf(stdout, "%li %llu %llu %i %i\n",
		((end.tv_sec - start.tv_sec) * 1000000) + (end.tv_usec - start.tv_usec),
		*COUNTER, *COUNTER - (nthreads * write_cycles),
		write_cycles, reads_per_write);
#endif

	return 0;
}

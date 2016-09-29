#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <poll.h>

/*

   rcu/qs

   This is the _Quiescent State_ implementation of RCU for
   concurrent data sharing across multiple threads.  Under
   QS, the threads are responsible for identifying their
   quiescent states (when they are no longer able to update
   the old copy of shared data).

   I could very well have gotten the implementation horribly
   wrong.  Please bear with me here...

 */

/* NUM_THREADS puts a fixed limit on the number of threads
   that can be spun up, such that we can use static arrays
   to imitate "weak" thread-local storage.

   This is one of many concessions that an application
   wishing to take advantage of quiescent state-based RCU
   needs to make, and why QSBR may not be universally
   applicable (i.e. via a library) */
#define NUM_THREADS 128

/* NO_THREAD is a _sigil value_ that identifies if a slot
   in a per-thread variable is available (set to NO_THREAD)
   or occupied (set to anything else). */
#define NO_THREAD ((pthread_t)(0UL))

/* Define a per-thread variable, for the calling thread.

   We imitate a weaker form of true thread-local storage by
   using an array of <type> things, with enough elements
   to store a distinct value for up to NUM_THREADS.

   The "weak" adjective refers to the ability for other
   threads to peer into the variable scope of other threads,
   which is impossible with POSIX pthreads thead-local
   implementations. */
#define DEFINE_PER_THREAD(type,name) static type __per_thread_ ## name[NUM_THREADS]
#define for_each_thread(t) for (t = 0; t < NUM_THREADS; t++)

#define mutex_lock(lock) ({ \
	if (pthread_mutex_lock(&(lock)) != 0) { \
		perror("pthread_mutex_lock"); \
		exit(3); \
	} \
})
#define mutex_unlock(lock) ({ \
	if (pthread_mutex_unlock(&(lock)) != 0) { \
		perror("pthread_mutex_unlock"); \
		exit(3); \
	} \
})

pthread_mutex_t tid_lock;
DEFINE_PER_THREAD(pthread_t, tid) = {0};

/* Retrieve a per-thread variable, for the calling thread. */
#define MY(var) THEIR(_thread_index(pthread_self()), var)

/* Retrieve a per-thread variable, for some other thread. */
#define THEIR(idx,var) __per_thread_##var[idx]

static inline
int _thread_index(pthread_t tid)
{
	int t;
	for_each_thread(t) {
		if (__per_thread_tid[t] == tid) {
			return t;
		}
	}
	fprintf(stderr, "unrecognized thread %p\n", tid);
	exit(1);
}

static inline
void register_thread()
{
	int t;

	mutex_lock(tid_lock);
	for_each_thread(t) {
		if (__per_thread_tid[t] == NO_THREAD) {
			__per_thread_tid[t] = pthread_self();
			mutex_unlock(tid_lock);
			return;
		}
	}
	fprintf(stderr, "self-destruct: unable to register %p - out of thread slots\n", pthread_self());
	exit(1);
}

static inline
void deregister_thread()
{
	MY(tid) = NO_THREAD;
}

/* Prevent compiler optimizations for memory access */
/*  (use of `volatile' keyword stops most sane compilers
     from duplicating de-duplicating variable / register access) */
#define ONCE(x) (*(volatile typeof(x) *)&(x))

/* Prevent compiler optimizations for memory reordering
   (this is a compile-time barrier to ensure that the optimizer
    doesn't try to "improve" how we access memory, and thereby
    ruin our carefully crafted concurrent code) */
#define barrier() asm volatile("" ::: "memory")

pthread_mutex_t rcu_gp_lock;
long rcu_gp_ctr = 0;
DEFINE_PER_THREAD(long, rcu_reader_qs_gp);

static inline void rcu_read_lock(void)
{
}

static inline void rcu_read_unlock(void)
{
}

static inline void rcu_quiescent_state(void)
{
	barrier();
	MY(rcu_reader_qs_gp) = ONCE(rcu_gp_ctr) + 1;
	barrier();
}

static inline void rcu_thread_offline(void)
{
	barrier();
	MY(rcu_reader_qs_gp) = ONCE(rcu_gp_ctr);
}
static inline void rcu_thread_online(void)
{
	MY(rcu_reader_qs_gp) = ONCE(rcu_gp_ctr) + 1;
	barrier();
}

static inline int rcu_gp_ongoing(int idx)
{
	return THEIR(idx, rcu_reader_qs_gp) & 1;
}

void synchronize_rcu(void)
{
	int t;

	barrier();
	mutex_lock(rcu_gp_lock);
	rcu_gp_ctr += 2;
	for_each_thread(t) {
		while (rcu_gp_ongoing(t) &&
		       ((THEIR(t, rcu_reader_qs_gp) - rcu_gp_ctr) < 0)) {
			poll(NULL, 0, 10);
			barrier();
		}
	}
	mutex_unlock(rcu_gp_lock);
	barrier();
}

/***********************************************************/

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
	unsigned long long *OLD, *NEW;

	register_thread();
	for (i = 0; i < params->write_cycles; i++) {
		for (j = 0; j < params->reads_per_write; j++) {
			rcu_read_lock();
			fprintf(stderr, "[t%d] read COUNTER at %llu\n", params->id, *(ONCE(COUNTER)));
			rcu_read_unlock();
		}

		NEW = malloc(sizeof(*NEW));
		if (!NEW) {
			perror("malloc");
			exit(3);
		}
		OLD = COUNTER;
		*NEW = *OLD;
		*NEW = *NEW + 1;
		COUNTER = NEW;
		fprintf(stderr, "[t%d] incremented COUNTER to %llu\n", params->id, *COUNTER);

		synchronize_rcu();
		free(OLD);
	}

	return _params;
}

static void rcu_init()
{
	int rc;

	rc = pthread_mutex_init(&rcu_gp_lock, NULL);
	if (rc != 0) {
		perror("pthread_mutex_init");
		exit(3);
	}

	rc = pthread_mutex_init(&tid_lock, NULL);
	if (rc != 0) {
		perror("pthread_mutex_init");
		exit(3);
	}
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

	rcu_init();
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

	fprintf(stdout, "%li %llu %llu %i %i\n",
		((end.tv_sec - start.tv_sec) * 1000000) + (end.tv_usec - start.tv_usec),
		*COUNTER, *COUNTER - (nthreads * write_cycles),
		write_cycles, reads_per_write);

	return 0;
}

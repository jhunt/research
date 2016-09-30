#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <poll.h>
#include "../api.h"

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

	lock(&tid_lock);
	for_each_thread(t) {
		if (__per_thread_tid[t] == NO_THREAD) {
			__per_thread_tid[t] = pthread_self();
			unlock(&tid_lock);
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
	lock(&rcu_gp_lock);
	rcu_gp_ctr += 2;
	for_each_thread(t) {
		while (rcu_gp_ongoing(t) &&
		       ((THEIR(t, rcu_reader_qs_gp) - rcu_gp_ctr) < 0)) {
			poll(NULL, 0, 10);
			barrier();
		}
	}
	unlock(&rcu_gp_lock);
	barrier();
}

/***********************************************************/

pthread_mutex_t     LOCK;
unsigned long long *COUNTER;

static inline
void writer(int cycles)
{
	unsigned long long *OLD, *NEW;

	while (cycles--) {

		NEW = xmalloc(sizeof(*NEW));

		lock(&LOCK);
		OLD = COUNTER;
		*NEW = *OLD + 1;
		COUNTER = NEW;
		unlock(&LOCK);

		fprintf(stderr, "[writer] incremented COUNTER to %llu\n", *NEW);

		synchronize_rcu();
		free(OLD);
	}
}

static inline
void sleep_ms(int ms)
{
	struct timespec tv;
	tv.tv_sec  = 0;
	tv.tv_nsec = ms * 1000000;
	nanosleep(&tv, NULL);
}

void* reader(void *_)
{
	int id = *(int *)_;

	register_thread();

	for (;;) {
		rcu_read_lock();
		fprintf(stderr, "[t%d] read COUNTER at %llu\n", id, *(ONCE(COUNTER)));
	//	sleep_ms(10);
		rcu_read_unlock();
		rcu_quiescent_state();
	}

	return _;
}

static void rcu_init()
{
	int rc;

	make_lock(&rcu_gp_lock);
	make_lock(&tid_lock);
}

int main(int argc, char **argv)
{
	struct timeval start, end;
	pthread_t tid;
	int rc, i, nthreads, cycles, *ids;
	void *status;

	if (argc != 3) {
		fprintf(stderr, "USAGE: %s THREADS CYCLES\n", argv[0]);
		exit(2);
	}

	nthreads = atoi(argv[1]);
	cycles   = atoi(argv[2]);
	if (nthreads < 1) {
		fprintf(stderr, "Invalid THREADS value %s (must be > 0)\n", argv[1]);
		fprintf(stderr, "USAGE: %s THREADS CYCLES\n", argv[0]);
		exit(2);
	}
	if (cycles < 1) {
		fprintf(stderr, "Invalid WRITE_CYCLES value %s (must be > 0)\n", argv[2]);
		fprintf(stderr, "USAGE: %s THREADS CYCLES\n", argv[0]);
		exit(2);
	}

	rcu_init();

	make_lock(&LOCK);
	ids = xcalloc(nthreads, sizeof(int));
	COUNTER = xcalloc(1, sizeof(unsigned long long));

	for (i = 0; i < nthreads; i++) {
		ids[i] = i+1;
		spin_up(reader, &ids[i]);
	}

	//sleep_ms(9);
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

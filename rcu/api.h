#include <pthread.h>

static inline void
spin_up(void *(*fn)(void *), void *restrict arg)
{
	pthread_t tid;
	if (pthread_create(&tid, NULL, fn, arg) != 0) {
		perror("pthread_create");
		exit(3);
	}
}

static inline void
make_lock(pthread_mutex_t *lck)
{
	if (pthread_mutex_init(lck, NULL) != 0) {
		perror("pthread_mutex_init");
		exit(3);
	}
}

static inline void
lock(pthread_mutex_t *lck) {
	if (pthread_mutex_lock(lck) != 0) { \
		perror("pthread_mutex_lock"); \
		exit(3); \
	} \
}

static inline void
unlock(pthread_mutex_t *lck) {
	if (pthread_mutex_unlock(lck) != 0) {
		perror("pthread_mutex_unlock");
		exit(3);
	}
}

#define xmalloc(n) xcalloc(1,(n))
static inline void*
xcalloc(size_t n, size_t len)
{
	void *p = calloc(n, len);
	if (!p) {
		perror("calloc");
		exit(3);
	}
	return p;
}

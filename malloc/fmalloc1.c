#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <unistd.h>

#define puts(x) write(2, (x), strlen(x))

static struct {
	int init;
	int left;
} FMALLOC = {0};
static int N = -1;

static inline void __init(void)
{
	if (!FMALLOC.init) {
		char *s = getenv("MALLOC_OK");
		FMALLOC.left = atoi(s ? s : "-1");

		FMALLOC.init = 1;
	}
}

static inline int __check(void)
{
	return FMALLOC.left == 0 ? 0 : (FMALLOC.left--, 1);
}

static inline void* __malloc(size_t n)
{
	if (!n)
		return NULL;

	void *p = sbrk(0);
	void *r = sbrk(n);
	if (r == (void*) -1)
		return NULL;

	assert(p == r); /* not thread-safe */
	return p;
}

void* malloc(size_t n)
{
	__init();
	return __check() ? __malloc(n) : NULL;
}

void* calloc(size_t n, size_t each)
{
	void *m = malloc(n * each);
	if (m)
		memset(m, 0, n * each);
	return m;
}

void *realloc(void *p, size_t n)
{
	return NULL;
}

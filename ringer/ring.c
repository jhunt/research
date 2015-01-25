#include "ring.h"
#include <stdlib.h>
#include <assert.h>

ring_t* ring_new(int size)
{
	assert(size > 0);

	ring_t *r = calloc(1, sizeof(ring_t));
	r->size = size;
	r->strings = calloc(size, sizeof(char *));
	return r;
}

void ring_free(ring_t *r)
{
	assert(r);

	char *v;
	for_each_in_ring(r, v)
		free(v);
	free(r);
}

int ring_isfull(ring_t *r)
{
	assert(r);
	return (r->tail + 1) % r->size == r->head;
}

int ring_isempty(ring_t *r)
{
	assert(r);
	return (r->head == r->tail);
}

int ring_add(ring_t *r, const char *s)
{
	assert(r);
	assert(s);

	if (r->strings[r->tail]) free(r->strings[r->tail]);
	r->strings[r->tail] = strdup(s);
	r->tail = ring_next(r, r->tail);
	return 0;
}

const char *ring_first(ring_t *r)
{
	assert(r);
	return r->strings[r->head];
}

const char *ring_last(ring_t *r)
{
	assert(r);
	return r->strings[ring_prev(r, r->head)];
}

#ifndef _RING_H
#define _RING_H

typedef struct {
	int    head;      /* index in strings[] of the list head */
	int    tail;      /* index in strings[] of the list tail */
	int    next;      /* state used by for_each_in_ring macro */
	int    size;      /* number of lines */
	char **strings;   /* buffer */
} ring_t;

#define ring_next(r,i) ((i) + 1)             % (r)->size
#define ring_prev(r,i) ((i) - 1 + (r)->size) % (r)->size

#define for_each_in_ring(r,v) for ((r)->next  = (r)->head, (v) = (r)->strings[(r)->next]; \
                                   (r)->next != (r)->tail; \
                                   (v) = (r)->strings[(r)->next  = ring_next((r), (r)->next)])

ring_t* ring_new(int size);
void ring_free(ring_t *r);
int ring_isempty(ring_t *r);
int ring_isfull(ring_t *r);

int ring_add(ring_t *r, const char *s);
const char *ring_first(ring_t *r);
const char *ring_last(ring_t *r);

#endif

#include "ring.h"
#include <ctap.h>

TESTS {
	subtest {
		ring_t *r = ring_new(5);
		ok(r, "ring_new() returns a new ring_t object");
		is_int(r->size, 5, "ring_t->size");
		is_int(r->head, 0, "ring_t->head");
		is_int(r->tail, 0, "ring_t->tail");
		ok(ring_isempty(r), "new ring is empty");
		ok(!ring_isfull(r), "new ring is not full");

		/* prev/next wraparound testing */
		is_int(ring_next(r, 0), 1, "next(0 [4]) = 1");
		is_int(ring_next(r, 1), 2, "next(1 [4]) = 2");
		is_int(ring_next(r, 2), 3, "next(2 [4]) = 3");
		is_int(ring_next(r, 3), 4, "next(3 [4]) = 4");
		is_int(ring_next(r, 4), 0, "next(4 [4]) = 0");

		is_int(ring_prev(r, 0), 4, "prev(0 [4]) = 4");
		is_int(ring_prev(r, 1), 0, "prev(0 [4]) = 0");
		is_int(ring_prev(r, 2), 1, "prev(0 [4]) = 1");
		is_int(ring_prev(r, 3), 2, "prev(0 [4]) = 2");
		is_int(ring_prev(r, 4), 3, "prev(0 [4]) = 3");

		ok(ring_add(r, "first string") == 0, "added 'first string' to ring");
		ok(!ring_isempty(r), "ring is no longer empty");
		ok(!ring_isfull(r), "ring is not yet full");

		ok(ring_add(r, "second string") == 0, "added 'second string' to ring");
		ok(ring_add(r, "third string")  == 0, "added 'third string' to ring");
		ok(!ring_isempty(r), "ring is no longer empty");
		ok(!ring_isfull(r), "ring is not yet full");

		ok(ring_add(r, "fourth string") == 0, "added 'fourth string' to ring");
		ok(!ring_isempty(r), "ring is no longer empty");
		ok(ring_isfull(r), "ring is full");

		ring_free(r);
	}

	subtest {
		int n; const char *v;
		ring_t *r = ring_new(3);
		if (!r) BAIL_OUT("failed to allocate a new ring");

		n = 0; for_each_in_ring(r, v) n++;
		is_int(n, 0, "for_each_in_ring handles empty rings");

		ok(ring_add(r, "A") == 0, " added 'A'");
		n = 0; for_each_in_ring(r, v) n++;
		is_int(n, 1, "for_each_in_ring handles [A]");
		is(ring_first(r), "A", "first([A]) == 'A'");
		is(ring_last(r),  "A",  "last([A]) == 'A'");

		ok(ring_add(r, "B") == 0, " added 'B'");
		n = 0; for_each_in_ring(r, v) n++;
		is_int(n, 2, "for_each_in_ring handles [A, B]");
		is(ring_first(r), "A", "first([A]) == 'A'");
		is(ring_last(r),  "B",  "last([B]) == 'B'");

		ok(ring_add(r, "C") == 0, " added 'C'");
		n = 0; for_each_in_ring(r, v) n++;
		is_int(n, 2, "for_each_in_ring handles [B, C]");
		is(ring_first(r), "B", "first([A]) == 'B'");
		is(ring_last(r),  "C",  "last([B]) == 'C'");
	}
}

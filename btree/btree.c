#include "btree.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#define _BTREE_MAXKEYS ((int)((BTREE_N) / 2))

static int
_find(struct btree *bt, uint32_t k)
{
	assert(bt != NULL);

	int lo, mid, hi;

	lo = -1;
	hi = bt->nkeys;
	while (lo != hi) {
		mid = (lo + hi) / 2;
		if (bt->keys[mid] == k) return mid;
		if (bt->keys[mid] >  k) hi = mid;
		else                    lo = mid;
	}
	return hi;
}

static int
_isfull(struct btree *bt)
{
	assert(bt != NULL);
	return bt->nkeys == _BTREE_MAXKEYS;
}

#if 0
static int
_split(struct btree *this, struct btree *parent)
{
	struct btree *l, *r;
	uint32_t mid;
	int i;

	assert(this != NULL);

	mid = this->keys[this->nkeys / 2];

	if (!parent) { /* split at root */
		l = btree_new(); assert(l != NULL);
		r = btree_new(); assert(l != NULL);
		mid = this->keys[this->nkeys / 2];

		/* copy lower half of keys / pointers to l */
		l->nkeys = this->nkeys / 2;
		for (i = 0; i < l->nkeys; i++) {
			l->keys[i] = this->keys[i];
			l->vals[i] = this->vals[i];
		}

		/* copy upper half of keys / pointers to r */
		r->nkeys = this->nkeys - l->nkeys;
		for (i = 0; i < r->nkeys; i++) {
			r->keys[i] = this->keys[i+l->nkeys];
			r->vals[i] = this->vals[i+l->nkeys];
		}
		r->vals[r->nkeys] = this->vals[this->nkeys];

		/* re-initialize this as [ l : mid : r ] */
		this->nkeys = 1;
		this->vals[0] = l;
		this->keys[0] = mid;
		this->vals[1] = r;

	} else { /* split at interior */
		r = btree_new();

		l->nkeys = this->nkeys / 2;
		for (i = 0; i < l->nkeys; i++) {
			l->keys[i] = this->keys[i];
			l->vals[i] = this->vals[i];
		}
	}

	return 0;
}
#endif

struct btree *
btree_new()
{
	struct btree * bt = calloc(1, sizeof(struct btree));
	assert(bt != NULL);

	bt->leaf = 1;
	return bt;
}

static void
_divide(struct btree *l, struct btree *r, int mid)
{
	l->nkeys = mid;
	r->nkeys = l->nkeys - mid - 1;

	memmove(r->keys, &l->keys[mid + 1], sizeof(uint32_t) *  r->nkeys     );
	memmove(r->vals, &l->vals[mid + 1], sizeof(void *)   * (r->nkeys + 1));
}

static void
_shiftr(struct btree *b, int n)
{
	memmove(&b->keys[n], &b->keys[n + 1], sizeof(uint32_t) * (BTREE_N - n    ));
	memmove(&b->vals[n], &b->vals[n + 1], sizeof(void *)   * (BTREE_N - n + 1));
}

static struct btree *
_clone(struct btree *bt)
{
	struct btree *r;

	r = btree_new();
	assert(r);

	r->leaf = bt->leaf;
	return r;
}

static struct btree *
_insert(struct btree *bt, uint32_t key, void *val, uint32_t *median)
{
	int i, mid;
	struct btree *r;

	i = _find(bt, key);

	if (i < bt->nkeys && bt->keys[i] == key) {
		bt->vals[i] = val;
		return NULL;
	}

	if (bt->leaf) {
		_shiftr(bt, i);
		bt->nkeys++;
		bt->keys[i] = key;
		bt->vals[i] = val;

	} else { /* insert in child */
		r = _insert(bt->vals[i], key, val, median);

		if (r) {
			_shiftr(bt, i);
			bt->nkeys++;
			bt->keys[i] = key;
			bt->vals[i] = r;
		}
	}

	/* split the node now, if it is full, to save complexity */
	if (_isfull(bt)) {
		mid = bt->nkeys / 2;
		*median = bt->keys[mid];

		r = _clone(bt);
		_divide(bt, r, mid);
		bt->nkeys = mid;
		return r;
	}

	return NULL;
}

int
btree_insert(struct btree *bt, uint32_t key, void *val)
{
	struct btree *l, *r;
	uint32_t m;

	assert(bt != NULL);
	assert(val != NULL);

	r = _insert(bt, key, val, &m);
	if (r) {
		/* "pivot" root to the left */
		l = btree_new();
		memmove(l, bt, sizeof(*bt));

		/* re-initialize root as [ l . m . r ] */
		bt->nkeys   = 1;
		bt->leaf    = 0;
		bt->vals[0] = l;
		bt->keys[0] = m;
		bt->vals[1] = r;
	}

	return 0;
}



#include <stdio.h>

int
main(int argc, char **argv)
{
	uint32_t ts;
	struct btree *bt;

	bt = btree_new();
	for (ts = 1234567890; ts < 1400000000; ts++) {
		if (ts % 1000 == 0) fprintf(stderr, "%d\n", ts);
		btree_insert(bt, ts, "test");
	}
	return 0;
}

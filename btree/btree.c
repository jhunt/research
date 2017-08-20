#include "btree.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#ifndef YEARS
#  define YEARS 2
#endif
#ifndef SPAN
#  define SPAN 30
#endif

static int
_find(struct btree *bt, uint32_t k)
{
	assert(bt != NULL);

	int lo, mid, hi;

	lo = -1;
	hi = bt->nkeys;
	while (lo + 1 < hi) {
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
	return bt->nkeys == BTREE_N;
}

static void
_print(struct btree *bt, int indent)
{
	int i;

	assert(bt != NULL);
	assert(indent >= 0);

	fprintf(stderr, "%*s[btree %p // %d keys]\n",
		indent, "", (void *)bt, bt->nkeys);

	for (i = 0; i < bt->nkeys; i++) {
		if (bt->leaf) {
			fprintf(stderr, "%*s[%03d] % 10d (= %lu)\n",
				indent + 2, "", i, bt->keys[i], bt->vals[i].data);
		} else {
			fprintf(stderr, "%*s[%03d] % 10d (%p) -->\n",
				indent + 2, "", i, bt->keys[i], (void *)bt->vals[i].child);
			_print(bt->vals[i].child, indent + 8);
		}

	}
	if (bt->leaf) {
		fprintf(stderr, "%*s[%03d]          ~ (= %lu)\n",
			indent + 2, "", i, bt->vals[bt->nkeys].data);
	} else {
		fprintf(stderr, "%*s[%03d]          ~ (%p) -->\n",
			indent + 2, "", i, (void *)bt->vals[bt->nkeys].child);
		_print(bt->vals[bt->nkeys].child, indent + 8);
	}
}

void
btree_print(struct btree *bt)
{
	_print(bt, 0);
}

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
	assert(l != NULL);
	assert(r != NULL);
	assert(mid != 0);
	assert(l->nkeys >= mid);

	r->nkeys = l->nkeys - mid - 1;
	l->nkeys = mid;

	memmove(r->keys, &l->keys[mid + 1], sizeof(uint32_t) *  r->nkeys     );
	memmove(r->vals, &l->vals[mid + 1], sizeof(void *)   * (r->nkeys + 1));

	return;
	/* optional */
	memset(&r->keys[r->nkeys], 0, BTREE_N - r->nkeys    );
	memset(&r->vals[r->nkeys], 0, BTREE_N - r->nkeys + 1);
}

static void
_shiftr(struct btree *b, int n)
{
	memmove(&b->keys[n + 1], &b->keys[n],     sizeof(uint32_t) * (b->nkeys - n));
	memmove(&b->vals[n + 2], &b->vals[n + 1], sizeof(void *)   * (b->nkeys - n));
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
_insert(struct btree *bt, uint32_t key, uint64_t val, uint32_t *median)
{
	int i, mid;
	struct btree *r;

	i = _find(bt, key);

	if (i < bt->nkeys && bt->keys[i] == key) {
		bt->vals[i].data = val;
		return NULL;
	}

	if (bt->leaf) {
		_shiftr(bt, i);
		bt->nkeys++;
		bt->keys[i]      = key;
		bt->vals[i].data = val;

	} else { /* insert in child */
		r = _insert(bt->vals[i].child, key, val, median);

		if (r) {
			_shiftr(bt, i);
			bt->nkeys++;
			bt->keys[i]         = *median;
			bt->vals[i+1].child =  r;
		}
	}

	/* split the node now, if it is full, to save complexity */
	if (_isfull(bt)) {
		mid = bt->nkeys * BTREE_S / 100;
		*median = bt->keys[mid];

		r = _clone(bt);
		_divide(bt, r, mid);
		return r;
	}

	return NULL;
}

int
btree_insert(struct btree *bt, uint32_t key, uint64_t val)
{
	struct btree *l, *r;
	uint32_t m;

	assert(bt != NULL);

	r = _insert(bt, key, val, &m);
	if (r) {
		/* "pivot" root to the left */
		l = btree_new();
		memmove(l, bt, sizeof(*bt));

		/* re-initialize root as [ l . m . r ] */
		bt->nkeys   = 1;
		bt->leaf    = 0;
		bt->vals[0].child = l;
		bt->keys[0]       = m;
		bt->vals[1].child = r;
	}

	return 0;
}

struct analysis {
	int nodes;
	int set;
	int depth;
};

static void
_analyze(struct btree *bt, struct analysis *a)
{
	int i;

	assert(bt != NULL);
	assert(a != NULL);

	a->nodes += 1;
	a->set += bt->nkeys + 1;

	if (!bt->leaf) {
		for (i = 0; i <= bt->nkeys; i++)
			_analyze(bt->vals[i].child, a);
	}
}

static void
btree_analyze(struct btree *bt)
{
	struct analysis a;
	double size;
	const char *unit;

	assert(bt != NULL);

	a.nodes = a.set = 0;
	_analyze(bt, &a);

	assert(a.nodes > 0);
	assert(a.set >= 0);

	a.depth = 0;
	while (bt && !bt->leaf) {
		a.depth++;
		bt = bt->vals[0].child;
	}

	fprintf(stderr, "N=%d, SFF=%0.2f, YEARS=%d, MIN=%d\n", BTREE_N, BTREE_S / 100.0, YEARS, SPAN);
	fprintf(stderr, "%d keys / %d nodes / %d levels\n", a.set, a.nodes, a.depth);
	fprintf(stderr, "%lu bytes per node\n", sizeof(struct btree));
	fprintf(stderr, "%0.3f%% slots filled\n", a.set * 100.0 / (a.nodes * BTREE_N));

	size = a.nodes * sizeof(struct btree);
	unit = "";

	fprintf(stderr, "%0.1f bytes (overhead) per key\n", size / a.set);

	if (size > 1024 * 1024 * 1024) {
		size /= 1024 * 1024 * 1024;
		unit = "G";
	}
	if (size > 1024 * 1024) {
		size /= 1024 * 1024;
		unit = "M";
	}
	if (size > 1024) {
		size /= 1024;
		unit = "K";
	}

	fprintf(stderr, "%0.1lf %sB\n", size, unit);
	fprintf(stderr, "\n");
}



#include <stdio.h>

int
main(int argc, char **argv)
{
	uint32_t ts;
	unsigned int i;
	struct btree *bt;

	bt = btree_new();

	for (i = 0; i < YEARS * 365U * 86400U; i += SPAN * 60) {
		ts = 1234567890 + i;
		if (ts %    1000000 == 0) fprintf(stderr, "%d\n", ts);
		btree_insert(bt, ts, ts);
	}
	btree_analyze(bt);

	return 0;
}

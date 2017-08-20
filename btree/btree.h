#ifndef BTREE_H
#define BTREE_H

#include <stdint.h>

//#define BTREE_N 340
#define BTREE_N 3

/* NOTE: this btree structure is not suitable
         for writing to disk and then mmap()-ing
         back in; it is just for exploration.
 */
struct btree {
	int       nkeys;
	int       leaf;
	uint32_t  keys[BTREE_N];
	void     *vals[BTREE_N + 1];
};

struct btree *
btree_new();

int
btree_insert(struct btree *bt, uint32_t key, void *val);

void *
btree_lookup(struct btree *bt, uint32_t key);

#endif

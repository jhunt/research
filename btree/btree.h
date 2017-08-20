#ifndef BTREE_H
#define BTREE_H

#include <stdint.h>

#ifndef BTREE_N
#  define BTREE_N 340
#endif

#ifndef BTREE_S
#  define BTREE_S 75
#endif

/* NOTE: this btree structure is not suitable
         for writing to disk and then mmap()-ing
         back in; it is just for exploration.
 */
struct btree {
	int       nkeys;
	int       leaf;
	uint32_t  keys[BTREE_N];
	union {
		uint64_t      data;
		struct btree *child;
	} vals[BTREE_N + 1];
};

struct btree *
btree_new();

int
btree_insert(struct btree *bt, uint32_t key, uint64_t val);

void *
btree_lookup(struct btree *bt, uint32_t key);

#endif

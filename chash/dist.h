#ifndef DIST_H
#define DIST_H

#include <stdint.h>

typedef struct {
	const char *name;
	uint8_t     weight;

	uint32_t    u; /* only used for analysis */
} node_t;

typedef struct {
	node_t   *node;
	uint64_t  key;
} vnode_t;

typedef struct {
	unsigned int spread; /* how many vnodes per weighting? */
	unsigned int len;
	vnode_t *vnodes;
} ring_t;

uint64_t murmur64a(const void *key, unsigned int len, unsigned int seed);
uint64_t murmur64b(const void *key, unsigned int len, unsigned int seed);

int ring_init(ring_t* ring, node_t *nodes, unsigned int n);
node_t* lookup(ring_t *ring, const char *key);

#endif

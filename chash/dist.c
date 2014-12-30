#include <vigor.h>
#include <string.h>
#include <assert.h>
#include "dist.h"

static uint64_t s_node_key(node_t *n, unsigned int i)
{
	char *k = string("%lu-%s", i, n);
	uint64_t h = murmur64a(k, strlen(k), 0);
	free(k);
	return h;
}

static int s_sort_vnodes(const void *a_, const void *b_)
{
	uint64_t a = ((const vnode_t *)a_)->key;
	uint64_t b = ((const vnode_t *)b_)->key;
	return a == b ? 0 : a > b ? 1 : -1;
}

int ring_init(ring_t* ring, node_t *nodes, unsigned int n)
{
	assert(ring != NULL);
	assert(nodes != NULL);
	assert(n != 0);

	unsigned int i, j, k;

	ring->len = 0;
	for (i = 0; i < n; i++)
		ring->len += ring->spread * nodes[i].weight;

	ring->vnodes = calloc(ring->len, sizeof(vnode_t));
	assert(ring->vnodes != NULL);
	for (i = k = 0; i < n && k < ring->len; i++) {
		nodes[i].u = 0;
		for (j = 0; j < ring->spread * nodes[i].weight; j++, k++) {
			ring->vnodes[k].node = &nodes[i];
			ring->vnodes[k].key  = s_node_key(&nodes[i], j);
		}
	}

	qsort(ring->vnodes, ring->len, sizeof(vnode_t), s_sort_vnodes);

	return 0;
}

node_t* lookup(ring_t *ring, const char *key)
{
	uint64_t h = murmur64a(key, strlen(key), 0);
	if (h > ring->vnodes[ring->len - 1].key)
		return ring->vnodes[0].node;

	unsigned int i;
	for (i = 0; i < ring->len; i++)
		if (h <= ring->vnodes[i].key)
			return ring->vnodes[i].node;

	return NULL;
}

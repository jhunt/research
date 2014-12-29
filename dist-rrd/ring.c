#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "dist.h"

int main(int argc, char **argv)
{
	int spread = (argc > 1 ? atoi(argv[1]) : 0);
	ring_t ring = { .spread = spread ? spread : 16 };
	node_t nodes[4] = {
		{ .name = "node01.ring.example.com", .weight = 1 },
		{ .name = "node02.ring.example.com", .weight = 2 },
		{ .name = "node03.ring.example.com", .weight = 1 },
		{ .name = "node04.ring.example.com", .weight = 1 }
	};

	int rc = ring_init(&ring, nodes, 4);
	assert(rc == 0);

	int i;
	for (i = 0; i < ring.len; i++)
		printf("%#018lx  %i  %s\n", ring.vnodes[i].key,
			ring.vnodes[i].node->weight, ring.vnodes[i].node->name);
	return 0;
}

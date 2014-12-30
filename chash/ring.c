#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <getopt.h>
#include "dist.h"

int main(int argc, char **argv)
{
	int spread;
	int quiet;

	int opt;
	while ((opt = getopt(argc, argv, "qs:")) != -1) {
		switch (opt) {
		case 'q': quiet = 1;             break;
		case 's': spread = atoi(optarg); break;
		default:
			fprintf(stderr, "USAGE: %s [-q] [-s spread]\n", argv[0]);
			return 1;
		}
	}

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
	for (i = 0; !quiet && i < ring.len; i++)
		printf("%#018lx  %i  %s\n", ring.vnodes[i].key,
			ring.vnodes[i].node->weight, ring.vnodes[i].node->name);

	int count = 0;
	char buf[8192];
	while ((fgets(buf, 8192, stdin))) {
		char *nl = strchr(buf, '\n');
		if (nl) *nl = '\0';

		node_t *n = lookup(&ring, buf);
		if (n) {
			count++;
			n->u++;
			if (!quiet)
				printf("key %-30s (%#018lx) is at %s (%i)\n",
					buf, murmur64a(buf, strlen(buf), 0),
					n ? n->name   : "(nil)",
					n ? n->weight : 0);
		} else {
			printf("key %s not found in ring!\n", buf);
		}
	}

	if (!quiet) printf("\n\n");
	for (i = 0; i < 4; i++) {
		printf("node %s (%i) accounts for % 5i/%i keys (%5.2lf%%)\n",
			nodes[i].name, nodes[i].weight, nodes[i].u, count, nodes[i].u * 100.0 / count);
	}
	return 0;
}

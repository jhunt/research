#include <stdio.h>
#include "bql.ll.h"

int main(int argc, char **argv) {
	void *state;

	state = lex_open("l/bql.ll");
	if (!state) {
		perror("lex_open");
		return 1;
	}

	while (lex(state)) {
		// ...
	}

	lex_close(state);
	return 0;
}

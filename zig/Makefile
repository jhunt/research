CFLAGS := -g

all: solver find-r
find-r: find-r.o
	$(CC) find-r.o -lm -o find-r
solver: solver.o
	$(CC) solver.o -lm -o solver

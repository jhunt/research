CFLAGS := -g -Wall

all: solver find-r
clean:
	rm -f find-r solver *.o

find-r: find-r.o
	$(CC) find-r.o -lm -o find-r
solver: solver.o
	$(CC) solver.o -lm -o solver

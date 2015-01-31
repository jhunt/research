CFLAGS := -Wall -g2 -O0 -Wno-unused

BINARIES :=
BINARIES += errno

all: $(BINARIES)
clean:
	rm -f *.o $(BINARIES)
errno.o: errno.c elist.h
elist.h: elist
	./mklist elist > $@

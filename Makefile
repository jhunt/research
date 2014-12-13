CFLAGS := -Wall -g2

BINARIES :=
BINARIES += null
BINARIES += statbuf
BINARIES += recur
BINARIES += heap
BINARIES += heap2

all: $(BINARIES)
clean:
	rm -f *.o $(BINARIES)

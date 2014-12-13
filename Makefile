CFLAGS := -Wall -g2 -O0

BINARIES :=
BINARIES += null
BINARIES += statbuf
BINARIES += recur
BINARIES += heap heap2 heap3

all: $(BINARIES)
clean:
	rm -f *.o $(BINARIES)

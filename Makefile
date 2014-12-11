CFLAGS := -Wall -g2

BINARIES :=
BINARIES += null
BINARIES += statbuf
BINARIES += recur

all: $(BINARIES)
clean:
	rm -f *.o $(BINARIES)

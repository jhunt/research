CFLAGS := -Wall -g2

BINARIES :=
BINARIES += null
BINARIES += statbuf

all: $(BINARIES)
clean:
	rm -f *.o $(BINARIES)

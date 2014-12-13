CFLAGS := -Wall -g2 -O0

BINARIES :=
BINARIES += null
BINARIES += statbuf
BINARIES += stack
BINARIES += heap
BINARIES += mmap mmap2 mmap3
BINARIES += fork

all: $(BINARIES)
clean:
	rm -f *.o $(BINARIES)

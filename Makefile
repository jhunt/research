CFLAGS := -Wall -g2 -O0 -Wno-unused

BINARIES :=
BINARIES += null
BINARIES += buf
BINARIES += stack
BINARIES += heap
BINARIES += mmap
BINARIES += fork

all: $(BINARIES)
clean:
	rm -f *.o $(BINARIES)

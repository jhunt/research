CFLAGS := $(shell pkg-config fuse --cflags)
LDLIBS := $(shell pkg-config fuse --libs)

nullfs: nullfs.o

clean:
	rm -f nullfs *.o

#
# fmalloc - 
#

all: victim libfmalloc1.so

victim: victim.c
	$(CC) -o $@ $+

libfmalloc1.so: fmalloc1.o
	$(CC) -shared -Wl,-soname,fmalloc1 -o $@ $+

%.o: %.c
	$(CC) -fPIC -c -o $@ $+

clean:
	rm -f fmalloc1.o
	rm -f victim
	rm -f libfmalloc.so

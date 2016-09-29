all: run-lock run-qs
clean:
	rm -f run-*
	rm -f */*.o

run-lock: lock/main.o
	$(CC) -o $@ $+

run-qs: qs/main.o
	$(CC) -o $@ $+

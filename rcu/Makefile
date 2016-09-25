all: run-lock
clean:
	rm -f run-*
	rm -f */*.o

run-lock: lock/main.o
	$(CC) -o $@ $+

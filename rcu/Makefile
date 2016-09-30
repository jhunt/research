all: run-lock run-qs
clean:
	rm -f run-*
	rm -f */*.o

run-lock: lock/main.o
	$(CC) -o $@ $+

run-qs: qs/main.o
	$(CC) -o $@ $+

data: trial.png
trial.png: out.data
	./plot > $@
	[[ -z "$$(command -v open)" ]] || open $@
out.data: run-lock run-qs report
	./report > out.data

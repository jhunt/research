CFLAGS := -g -Wall
LDLIBS := -lvigor
default: regm asm
regm: regm.o
asm: asm.o
asm.o: asm0.c asm.c

opcodes.h: opcodes.yml gencode
	./gencode opcodes > $@
asm0.c: opcodes.yml gencode
	./gencode asm > $@

clean:
	rm *.o asm regm

test: regm
	./regm

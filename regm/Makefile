CFLAGS := -g -Wall
LDLIBS := -lvigor
default: regm asm
regm: regm.o
asm: asm.o

opcodes.h: opcodes.yml bits.pl
	perl ./bits.pl > opcodes.h

clean:
	rm *.o asm regm

test: regm
	./regm

CFLAGS := -g -Wall
LDLIBS := -lvigor
default: regm asm
regm: regm.o
asm: asm.o

opcodes.h: opcodes.yml gencode
	./gencode opcodes > $@

clean:
	rm *.o asm regm

test: regm
	./regm

pn/%.b: pn/%.pn asm
	./asm <$< >$@

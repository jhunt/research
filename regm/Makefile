CFLAGS := -g -Wall
LDLIBS := -lvigor

BINARIES :=
BINARIES += regm
BINARIES += asm

EXAMPLES_SRC := $(shell ls -1 pn/*.pn)
EXAMPLES_BIN := $(shell ls -1 pn/*.pn | sed -e 's/\.pn/.b/')

################################################
# DUMMY TARGETS

default: $(BINARIES)
clean:
	rm *.o asm regm
	rm -f $(EXAMPLES_BIN)

test: default
	./regm

examples: $(EXAMPLES_BIN)


################################################
# BINARY targets

regm: regm.o
asm: asm.o

################################################
# SUPPORTING targets

opcodes.h: opcodes.yml gencode
	./gencode opcodes > $@

pn/%.b: pn/%.pn asm
	./asm <$< >$@

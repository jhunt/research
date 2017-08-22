hello: hello.o
	ld -o $@ $+

%.o: %.s
	nasm -f elf64 -g -F stabs $+

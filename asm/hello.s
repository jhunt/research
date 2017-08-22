; hello.s
; 21 Aug 2017 - jrh
;
; A simple Hello, World! in Intel x86-64 assembly
;
; Compile with:
;    nasm -f elf -g -F stabs hello.s
;    ld -o hello hello.o
;

section .bss

section .data
	hello_str db "Hello, World!",10
	hello_len equ $-hello_str

section .text
	global _start

_start:
	nop ; hi gdb!

	; write(0, hello_str, hello_len)
	mov rax,1         ; system call number
	mov rdi,1         ; standard output
	mov rsi,hello_str ; buffer to print
	mov rdx,hello_len ; bytes to print
	syscall

	; exit(1)
	mov rax,60 ; system call number
	mov rdi,0  ; exit code
	syscall    ; trap into kernel mode

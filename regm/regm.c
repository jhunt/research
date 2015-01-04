#include "regm.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <ctype.h>

#include "opcodes.h"

static void save_state(vm_t *vm)
{
	int i;
	for (i = 0; i < NREGS; i++)
		push(&vm->dstack, vm->r[i]);
}

static void restore_state(vm_t *vm)
{
	int i;
	for (i = NREGS - 1; i >= 0; i--)
		vm->r[i] = pop(&vm->dstack);
}

static void dump(FILE *io, vm_t *vm)
{
	fprintf(io, "\n");
	fprintf(io, "    ---------------------------------------------------------------------\n");
	fprintf(io, "    %%%c [ %08x ]   %%%c [ %08x ]   %%%c [ %08x ]   %%%c [ %08x ]\n",
		'a', vm->r[0],  'b', vm->r[1], 'c', vm->r[2],  'd', vm->r[3]);
	fprintf(io, "    %%%c [ %08x ]   %%%c [ %08x ]   %%%c [ %08x ]   %%%c [ %08x ]\n",
		'e', vm->r[4],  'f', vm->r[5], 'g', vm->r[6],  'h', vm->r[7]);
	fprintf(io, "    %%%c [ %08x ]   %%%c [ %08x ]   %%%c [ %08x ]   %%%c [ %08x ]\n",
		'i', vm->r[8],  'j', vm->r[9], 'k', vm->r[10], 'l', vm->r[11]);
	fprintf(io, "    %%%c [ %08x ]   %%%c [ %08x ]   %%%c [ %08x ]   %%%c [ %08x ]\n",
		'm', vm->r[12], 'n', vm->r[13], 'o', vm->r[14], 'p', vm->r[15]);
	fprintf(io, "\n");

	fprintf(io, "    acc: %08x\n", vm->acc);
	fprintf(io, "     pc: %08x\n", vm->pc);
	fprintf(io, "\n");

	int i;
	if (vm->dstack.top == 0) {
		fprintf(io, "    data stack: <empty>\n");
	} else {
		fprintf(io, "    data stack: | %04x | 0\n", vm->dstack.val[0]);
		for (i = 1; i < vm->dstack.top; i++)
			fprintf(io, "                | %04x | %i\n", vm->dstack.val[i], i);
	}

	if (vm->istack.top == 0) {
		fprintf(io, "    inst stack: <empty>\n");
	} else {
		fprintf(io, "    inst stack: | %04x | 0\n", vm->istack.val[0]);
		for (i = 1; i < vm->istack.top; i++)
			fprintf(io, "                | %04x | %i\n", vm->istack.val[i], i);
	}

	fprintf(io, "    ---------------------------------------------------------------------\n\n");
}

static int ischar(char c, const char *accept)
{
	while (*accept && *accept != c) accept++;
	return *accept == c;
}

static void vm_fprintf(vm_t *vm, FILE *out, const char *fmt)
{
	char reg, type, next, *a, *b, *buf;
	a = b = buf = strdup(fmt);

#define ADVANCE do { \
	b++; \
	if (!*b) { \
		fprintf(stderr, "<< unexpected end of format string >>\n"); \
		free(buf); return; \
	} \
} while (0)

	while (*b) {
		if (*b == '%') {
			if (*(b+1) == '%') { /* %% == % */
				*++b = '\0';
				fprintf(out, "%s", a);
				a = ++b;
				continue;
			}
			*b = '\0';
			ADVANCE;
			fprintf(out, "%s", a);

			if (*b != '[') {
				fprintf(stderr, "<< invalid format specifier >>\n");
				goto bail;
			}

			ADVANCE;
			if (*b < 'a' || *b >= 'a' + NREGS) {
				fprintf(stderr, "<< invalid register %%%c >>\n", *b);
				goto bail;
			}
			reg = *b;

			ADVANCE;
			if (*b != ']') {
				fprintf(stderr, "<< invalid format specifier >>\n");
				goto bail;
			}
			a = b; *a = '%';

			while (*b && !ischar(*b, "sdiouxX"))
				ADVANCE;
			type = *b; b++;
			next = *b; *b = '\0';

			switch (type) {
			case 's':
				fprintf(out, a, (char *)(vm->code + vm->r[reg - 'a']));
				break;
			default:
				fprintf(out, a, vm->r[reg - 'a']);
				break;
			}

			*b = next;
			a = b;
			continue;
		}
		b++;
	}
	if (*a)
		fprintf(out, "%s", a);

#undef ADVANCE

bail:
	free(buf);
	return;
}

int vm_reset(vm_t *vm)
{
	assert(vm);
	memset(vm, 0, sizeof(vm_t));
	return 0;
}

int vm_prime(vm_t *vm, byte_t *code, size_t len)
{
	assert(vm);
	assert(code);
	assert(len > 1);

	vm->code = code;
	vm->codesize = len;
	return 0;
}

#define B_ERR(...) do { \
	fprintf(stderr, "regm bytecode error: "); \
	fprintf(stderr, __VA_ARGS__); \
	fprintf(stderr, "\n"); \
	return -1; \
} while (0)

#define ARG0(s) do { if ( f1 ||  f2) B_ERR(s " takes no operands");            } while (0)
#define ARG1(s) do { if (!f1 ||  f2) B_ERR(s " requires exactly one operand"); } while (0)
#define ARG2(s) do { if (!f1 || !f2) B_ERR(s " requires two operands");        } while (0)

#define TYPE_LITERAL  0x1
#define TYPE_REGISTER 0x2
#define TYPE_ADDRESS  0x3

#define is_value(fl)    ((fl) == TYPE_LITERAL)
#define is_address(fl)  ((fl) == TYPE_ADDRESS)
#define is_register(fl) ((fl) == TYPE_REGISTER)

const char *stringv(vm_t *vm, byte_t type, dword_t arg)
{
	switch (type) {
	case TYPE_ADDRESS:
		if (arg >= vm->codesize) return NULL;
		return (char *)((uintptr_t)vm->code + arg);

	case TYPE_REGISTER:
		if (arg > NREGS) return NULL;
		return (char *)((uintptr_t)vm->code + vm->r[arg]);

	default: return NULL;
	}
}
#define BADVALUE 0xffffffff
dword_t value_of(vm_t *vm, byte_t type, dword_t arg)
{
	switch (type) {
	case TYPE_LITERAL:
		return arg;

	case TYPE_ADDRESS:
		if (arg >= vm->codesize) return BADVALUE;
		return arg;

	case TYPE_REGISTER:
		if (arg > NREGS) return BADVALUE;
		return vm->r[arg];

	default:
		return BADVALUE;
	}
}

int vm_exec(vm_t *vm)
{
	byte_t op, f1, f2;
	dword_t oper1, oper2;
	vm->pc = 0;

	for (;;) {
		oper1 = oper2 = 0;
		op = vm->code[vm->pc++];
		f1 = HI_NYBLE(vm->code[vm->pc]);
		f2 = LO_NYBLE(vm->code[vm->pc]);
		vm->pc++;

		if (f2 && !f1)
			B_ERR("corrupt operands mask detected; f1=%02x, f2=%02x", f1, f2);

		if (f1) {
			oper1 = DWORD(vm->code[vm->pc + 0],
			              vm->code[vm->pc + 1],
			              vm->code[vm->pc + 2],
			              vm->code[vm->pc + 3]);
			vm->pc += 4;
		}

		if (f2) {
			oper2 = DWORD(vm->code[vm->pc + 0],
			              vm->code[vm->pc + 1],
			              vm->code[vm->pc + 2],
			              vm->code[vm->pc + 3]);
			vm->pc += 4;
		}

		switch (op) {
		case NOOP:
			break;

		case PUSH:
			ARG1("push");
			if (!is_register(f1))
				B_ERR("push requires a register index for operand 1");
			if (oper1 > NREGS)
				B_ERR("register %08x is out of bounds", oper1);

			push(&vm->dstack, vm->r[oper1]);
			break;

		case POP:
			ARG1("pop");
			if (!is_register(f1))
				B_ERR("pop requires a register index for operand 1");
			if (oper1 > NREGS)
				B_ERR("register %08x is out of bounds", oper1);

			vm->r[oper1] = pop(&vm->dstack);
			break;

		case SET:
			ARG2("set");
			if (!is_register(f1))
				B_ERR("set requires a register index for operand 1");
			if (oper1 > NREGS)
				B_ERR("register %08x is out of bounds", oper1);

			vm->r[oper1] = value_of(vm, f2, oper2);
			break;

		case SWAP:
			ARG2("swap");
			if (!is_register(f1))
				B_ERR("swap requires a register index for operand 1");
			if (oper1 > NREGS)
				B_ERR("register %08x is out of bounds", oper1);

			if (!is_register(f2))
				B_ERR("swap requires a register index for operand 2");
			if (oper2 > NREGS)
				B_ERR("register %08x is out of bounds", oper2);

			if (oper1 == oper2)
				B_ERR("swap requires distinct registers for operands");

			vm->r[oper1] ^= vm->r[oper2];
			vm->r[oper2] ^= vm->r[oper1];
			vm->r[oper1] ^= vm->r[oper2];
			break;

		case ADD:
			ARG2("add");
			if (!is_register(f1))
				B_ERR("add requires a register index for operand 1");
			if (oper1 > NREGS)
				B_ERR("register %08x is out of bounds", oper1);

			vm->r[oper1] += value_of(vm, f2, oper2);
			break;

		case SUB:
			ARG2("sub");
			if (!is_register(f1))
				B_ERR("sub requires a register index for operand 1");
			if (oper1 > NREGS)
				B_ERR("register %08x is out of bounds", oper1);

			vm->r[oper1] -= value_of(vm, f2, oper2);
			break;

		case MULT:
			ARG2("mult");
			if (!is_register(f1))
				B_ERR("mult requires a register index for operand 1");
			if (oper1 > NREGS)
				B_ERR("register %08x is out of bounds", oper1);

			vm->r[oper1] *= value_of(vm, f2, oper2);
			break;

		case DIV:
			ARG2("div");
			if (!is_register(f1))
				B_ERR("div requires a register index for operand 1");
			if (oper1 > NREGS)
				B_ERR("register %08x is out of bounds", oper1);

			vm->r[oper1] /= value_of(vm, f2, oper2);
			break;

		case MOD:
			ARG2("mod");
			if (!is_register(f1))
				B_ERR("mod requires a register index for operand 1");
			if (oper1 > NREGS)
				B_ERR("register %08x is out of bounds", oper1);

			vm->r[oper1] %= value_of(vm, f2, oper2);
			break;

		case CALL:
			ARG1("call");
			if (!is_address(f1))
				B_ERR("call requires an address for operand 1");

			save_state(vm);
			push(&vm->istack, vm->pc);
			vm->pc = oper1;
			break;

		case RET:
			if (f1) {
				ARG1("ret");
				vm->acc = value_of(vm, f1, oper1);
			} else {
				ARG0("ret");
			}
			if (empty(&vm->istack))
				return 0; /* last RET == HALT */
			vm->pc = pop(&vm->istack);
			restore_state(vm);
			break;

		case CMP:
			ARG2("cmp");
			vm->acc = value_of(vm, f1, oper1) - value_of(vm, f2, oper2);
			break;

		case STRCMP:
			ARG2("strcmp");
			vm->acc = strcmp(stringv(vm, f1, oper1), stringv(vm, f1, oper2));
			break;

		case JMP:
			ARG1("jmp");
			vm->pc = oper1;
			break;

		case JZ:
			ARG1("jz");
			if (vm->acc == 0) vm->pc = value_of(vm, f1, oper1);
			break;

		case JNZ:
			ARG1("jnz");
			if (vm->acc != 0) vm->pc = value_of(vm, f1, oper1);
			break;

		case ECHO:
			ARG1("echo");
			vm_fprintf(vm, stdout, stringv(vm, f1, oper1));
			break;

		case ERR:
			ARG1("err");
			vm_fprintf(vm, stderr, stringv(vm, f1, oper1));
			fprintf(stderr, "\n");
			break;

		case PERROR:
			ARG1("perror");
			vm_fprintf(vm, stderr, stringv(vm, f1, oper1));
			fprintf(stderr, ": (%i) %s\n", errno, strerror(errno));
			break;

		case BAIL:
			ARG0("bail");
			printf("bail\n"); /* FIXME: not implemented */
			break;

		case MARK:
			ARG0("mark");
			printf("mark\n"); /* FIXME: not implemented */
			break;

		case FS_STAT:
			printf("fs.stat\n"); /* FIXME: not implemented */
			break;

		case FS_FILE_P:
			printf("fs.file?\n"); /* FIXME: not implemented */
			break;

		case FS_SYMLINK_P:
			printf("fs.symlink?\n"); /* FIXME: not implemented */
			break;

		case FS_TOUCH:
			printf("fs.touch\n"); /* FIXME: not implemented */
			break;

		case FS_UNLINK:
			printf("fs.unlink\n"); /* FIXME: not implemented */
			break;

		case FS_RENAME:
			printf("fs.rename\n"); /* FIXME: not implemented */
			break;

		case FS_CHOWN:
			printf("fs.chown\n"); /* FIXME: not implemented */
			break;

		case FS_CHGRP:
			printf("fs.chgrp\n"); /* FIXME: not implemented */
			break;

		case FS_CHMOD:
			printf("fs.chmod\n"); /* FIXME: not implemented */
			break;

		case FS_SHA1:
			printf("fs.sha1\n"); /* FIXME: not implemented */
			break;

		case GETFILE:
			printf("getfile\n"); /* FIXME: not implemented */
			break;

		case GETUID:
			printf("getuid\n"); /* FIXME: not implemented */
			break;

		case GETGID:
			printf("getgid\n"); /* FIXME: not implemented */
			break;

		case EXEC:
			printf("exec\n"); /* FIXME: not implemented */
			break;

		case HALT:
			ARG0("halt");
			return 0;

		case DUMP:
			ARG0("dump");
			dump(stderr, vm);
			break;

		default:
			B_ERR("unknown operand %02x", op);
			return -1;
		}
	}
}

int empty(stack_t *st)
{
	return st->top == 0;
}

int push(stack_t *st, dword_t value)
{
	assert(st);
	if (st->top == 254) {
		fprintf(stderr, "stack overflow!\n");
		abort();
	}

	st->val[st->top++] = value;
	return 0;
}

dword_t pop(stack_t *st)
{
	assert(st);
	if (empty(st)) {
		fprintf(stderr, "stack underflow!\n");
		abort();
	}

	return st->val[--st->top];
}

int main (int argc, char **argv)
{
	if (argc != 2) {
		fprintf(stderr, "USAGE: %s asm.b\n", argv[0]);
		return 1;
	}

	int rc, fd = open(argv[1], O_RDONLY);
	if (fd < 0) {
		perror(argv[1]);
		return 1;
	}
	off_t n = lseek(fd, 0, SEEK_END);
	if (n < 0) {
		perror(argv[1]);
		return 1;
	}
	byte_t *code = mmap(NULL, n, PROT_READ, MAP_PRIVATE, fd, 0);
	if (!code) {
		perror(argv[1]);
		return 1;
	}

	vm_t vm;
	rc = vm_reset(&vm);
	assert(rc == 0);

	rc = vm_prime(&vm, code, n);
	assert(rc == 0);

	rc = vm_exec(&vm);
	assert(rc == 0);

	return 0;
}

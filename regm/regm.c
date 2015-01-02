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

#define BADVALUE 0xffffffff
dword_t value_of(vm_t *vm, byte_t type, dword_t arg)
{
	switch (type) {
	case TYPE_LITERAL:
		return arg;

	case TYPE_ADDRESS:
		if (arg >= vm->codesize)
			return BADVALUE;
		return arg;

	case TYPE_REGISTER:
		if (arg > NREGS) {
			return BADVALUE;
		}
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

		//printf("[[ %02x %01x %01x ]]\n", op, f1, f2);

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
			printf("noop\n");
			break;

		case PUSH:
			ARG1("push");
			if (!is_register(f1))
				B_ERR("push requires a register index for operand 1");
			if (oper1 > NREGS)
				B_ERR("register %08x is out of bounds", oper1);

			printf("push (%08x)\n", oper1);
			push(&vm->dstack, vm->r[oper1]);
			break;

		case POP:
			ARG1("pop");
			if (!is_register(f1))
				B_ERR("pop requires a register index for operand 1");
			if (oper1 > NREGS)
				B_ERR("register %08x is out of bounds", oper1);

			printf("pop (%08x)\n", oper1);
			vm->r[oper1] = pop(&vm->dstack);
			break;

		case SET:
			ARG2("set");
			if (!is_register(f1))
				B_ERR("set requires a register index for operand 1");
			if (oper1 > NREGS)
				B_ERR("register %08x is out of bounds", oper1);

			vm->r[oper1] = value_of(vm, f2, oper2);
			printf("set (%08x) (%08x)\n", oper1, oper2);
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
			printf("swap (%08x) (%08x)\n", oper1, oper2);
			break;

		case CALL:
			ARG1("call");
			if (!is_address(f1))
				B_ERR("call requires an address for operand 1");
			printf("call (%08x)\n", oper1);

			save_state(vm);
			push(&vm->istack, vm->pc);
			vm->pc = oper1;
			break;

		case RET:
			if (f1) {
				ARG1("ret");
				printf("ret (%08x)\n", oper1);
			} else {
				ARG0("ret");
				printf("ret\n");
			}
			if (empty(&vm->istack))
				return 0; /* last RET == HALT */
			vm->pc = pop(&vm->istack);
			restore_state(vm);
			break;

		case CMP:
			ARG2("cmp");
			printf("cmp (%08x) (%08x)\n", oper1, oper2);
			break;

		case STRCMP:
			ARG2("strcmp");
			printf("strcmp (%08x) (%08x)\n", oper1, oper2);
			break;

		case JMP:
			ARG1("jmp");
			printf("jmp (%08x)\n", oper1);
			vm->pc = oper1;
			break;

		case JZ:
			ARG1("jz");
			printf("jz (%08x)\n", oper1);
			break;

		case JNZ:
			ARG1("jnz");
			printf("jnz (%08x)\n", oper1);
			break;

		case ECHO:
			ARG1("echo");
			printf("echo (%08x)\n", oper1);
			printf((char *)(vm->code + value_of(vm, f1, oper1)),
				vm->r[0], vm->r[1], vm->r[2], vm->r[3], vm->r[4], vm->r[5]);
			break;

		case ERR:
			ARG1("err");
			printf("err (%08x)\n", oper1);
			break;

		case PERROR:
			ARG1("perror");
			printf("perror (%08x)\n", oper1);
			break;

		case BAIL:
			ARG0("bail");
			printf("bail\n");
			break;

		case MARK:
			ARG0("mark");
			printf("mark\n");
			break;

		case FSTAT:
			printf("fstat\n");
			break;

		case ISFILE:
			printf("isfile\n");
			break;

		case ISLINK:
			printf("islink\n");
			break;

		case TOUCH:
			printf("touch\n");
			break;

		case UNLINK:
			printf("unlink\n");
			break;

		case RENAME:
			printf("rename\n");
			break;

		case CHOWN:
			printf("chown\n");
			break;

		case CHGRP:
			printf("chgrp\n");
			break;

		case CHMOD:
			printf("chmod\n");
			break;

		case FSHA1:
			printf("fsha1\n");
			break;

		case GETFILE:
			printf("getfile\n");
			break;

		case GETUID:
			printf("getuid\n");
			break;

		case GETGID:
			printf("getgid\n");
			break;

		case EXEC:
			printf("exec\n");
			break;

		case HALT:
			ARG0("halt");
			printf("halt\n");
			return 0;

		case DUMP:
			ARG0("dump");
			printf("dump\n");
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

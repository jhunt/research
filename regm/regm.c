#include "regm.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "opcodes.h"

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

#define HI_NYBLE(_) (((_) >> 4) & 0x0f)
#define LO_NYBLE(_) ( (_)       & 0x0f)
#define HI_BYTE(_)  (((_) >> 8) & 0xff);
#define LO_BYTE(_)  ( (_)       & 0xff);
#define WORD(a,b) ((a << 8) | (b))
#define DWORD(a,b,c,d) ((a << 24) | (b << 16) | (c << 8) | (d))

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
		if (arg >= vm->heapsize)
			return BADVALUE;
		return vm->heap[arg];

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

		case CALL:
			ARG1("call");
			printf("call (%08x)\n", oper1);
			break;

		case RET:
			ARG0("ret");
			printf("ret\n");
			break;

		case CMP:
			ARG2("cmp");
			printf("cmp (%08x) (%08x)\n", oper1, oper2);
			break;

		case JZ:
			ARG1("jz");
			printf("jz (%08x)\n", oper1);
			break;

		case JNZ:
			ARG1("jnz");
			printf("jnz (%08x)\n", oper1);
			break;

		case JMP:
			ARG1("jmp");
			printf("jmp (%08x)\n", oper1);
			break;

		case HALT:
			ARG0("halt");
			printf("halt\n");
			return 0;

		case DUMP:
			ARG0("dump");
			printf("dump\n");

			fprintf(stderr, "\n");
			fprintf(stderr, "    ---------------------------------------------------------------------\n");
			fprintf(stderr, "    %%%c [ %08x ]   %%%c [ %08x ]   %%%c [ %08x ]   %%%c [ %08x ]\n",
				'a', vm->r[0],  'b', vm->r[1], 'c', vm->r[2],  'd', vm->r[3]);
			fprintf(stderr, "    %%%c [ %08x ]   %%%c [ %08x ]   %%%c [ %08x ]   %%%c [ %08x ]\n",
				'e', vm->r[4],  'f', vm->r[5], 'g', vm->r[6],  'h', vm->r[7]);
			fprintf(stderr, "    %%%c [ %08x ]   %%%c [ %08x ]   %%%c [ %08x ]   %%%c [ %08x ]\n",
				'i', vm->r[8],  'j', vm->r[9], 'k', vm->r[10], 'l', vm->r[11]);
			fprintf(stderr, "    %%%c [ %08x ]   %%%c [ %08x ]   %%%c [ %08x ]   %%%c [ %08x ]\n",
				'm', vm->r[12], 'n', vm->r[13], 'o', vm->r[14], 'p', vm->r[15]);
			fprintf(stderr, "\n");

			fprintf(stderr, "    acc: %08x\n", vm->acc);
			fprintf(stderr, "     pc: %08x\n", vm->pc);
			fprintf(stderr, "\n");

			fprintf(stderr, "    data stack: | %04x | 0\n", vm->dstack.val[0]);
			int i;
			for (i = 1; i < vm->dstack.top; i++)
				fprintf(stderr, "                | %04x | %i\n", vm->dstack.val[i], i);

			fprintf(stderr, "    ---------------------------------------------------------------------\n\n");
			break;



		default:
			B_ERR("unknown operand %02x", op);
			return -1;
		}
	}
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
	if (st->top == 0) {
		fprintf(stderr, "stack underflow!\n");
		abort();
	}

	return st->val[st->top--];
}





int main (int argc, char **argv)
{
	int rc;
	size_t n;
	byte_t code[] =
		"\x00\0"                       /*   2 */
		"\x03\x21\x00\x00\x00\x04"     /*   8 */
		        "\x53\x52\x51\x50"     /*  12 */
		"\x01\x20\x00\x00\x00\x04"     /*  18 */
		"\x0b\0"                       /*  20 */
		"\x04\x10\x68\x67\x66\x65"     /*  26 */
		"\x05\0"                       /*  28 */
		"\x06\x11\x51\x52\x53\x54"     /*  34 */
		        "\x41\x42\x43\x44"     /*  38 */
		"\x07\x10\x39\x38\x37\x36"     /*  44 */
		"\x08\x10\x29\x28\x27\x26"     /*  50 */
		"\x09\x10\x19\x18\x17\x16"     /*  56 */
		"\x0a\0"                       /*  58 */
		"\x01\x20\x00\x00\x00\x01"     /*  64 - never reached! */
		""; n = 64;

	vm_t vm;
	rc = vm_reset(&vm);
	assert(rc == 0);

	rc = vm_prime(&vm, code, n);
	assert(rc == 0);

	rc = vm_exec(&vm);
	assert(rc == 0);

	return 0;
}

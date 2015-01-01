#include "regm.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

/* OPERANDS */
#define NOOP  0 /* does nothing                - noop      */
#define PUSH  1 /* push a register onto stack  - push %a   */
#define POP   2 /* pop stack top into register - pop %a    */
#define SET   3 /* set register value          - set %a $v */
#define CALL  4 /* call function               - call [fn] */
#define RET   5 /* return from function        - ret       */
#define CMP   6 /* compare integers            - cmp %a %b */
#define JZ    7 /* jump if accumulator == 0    - jz @addr  */
#define JNZ   8 /* jump if accumulator != 0    - jnz @addr */
#define JMP   9 /* unconditional jump          - jmp @addr */
#define HALT 10 /* halt the virtual machine    - halt      */

#define T_LITERAL  0x1
#define T_REGISTER 0x2
#define T_ADDRESS  0x3

static const char *OP_NAMES[] = {
	"noop",
	"push",
	"pop",
	"set",
	"call",
	"ret",
	"cmp",
	"jz",
	"jnz",
	NULL,
};

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

#define is_value(fl)    ((fl) == T_LITERAL)
#define is_address(fl)  ((fl) == T_ADDRESS)
#define is_register(fl) ((fl) == T_REGISTER)

#define BADVALUE 0xffffffff
dword_t value_of(vm_t *vm, byte_t type, dword_t arg)
{
	switch (type) {
	case T_LITERAL:
		return arg;

	case T_ADDRESS:
		if (arg >= vm->heapsize)
			return BADVALUE;
		return vm->heap[arg];

	case T_REGISTER:
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
			oper1 = DWORD(vm->code[vm->pc++],
			              vm->code[vm->pc++],
			              vm->code[vm->pc++],
			              vm->code[vm->pc++]);
		}

		if (f2) {
			oper2 = DWORD(vm->code[vm->pc++],
			              vm->code[vm->pc++],
			              vm->code[vm->pc++],
			              vm->code[vm->pc++]);
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
			break;

		case POP:
			ARG1("pop");
			if (!is_register(f1))
				B_ERR("pop requires a register index for operand 1");
			if (oper1 > NREGS)
				B_ERR("register %08x is out of bounds", oper1);

			printf("pop (%08x)\n", oper1);
			break;

		case SET:
			ARG2("set");
			if (!is_register(f1))
				B_ERR("set requires a register index for operand 1");
			if (oper1 > NREGS)
				B_ERR("register %08x is out of bounds", oper1);
			if (!is_value(f2))
				B_ERR("set requires a literal value for operand 2");

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

		default:
			B_ERR("unknown operand %02x", op);
			return -1;
		}
	}
}




int main (int argc, char **argv)
{
	int rc;
	size_t n;
	byte_t code[] =
		"\x00\0"                       /*   2 */
		"\x01\x20\x00\x00\x00\x01"     /*   8 */
		"\x02\x20\x00\x00\x00\x01"     /*  14 */
		"\x03\x21\x00\x00\x00\x04"     /*  20 */
		        "\x53\x52\x51\x50"     /*  24 */
		"\x04\x10\x68\x67\x66\x65"     /*  28 */
		"\x05\0"                       /*  30 */
		"\x06\x11\x51\x52\x53\x54"     /*  34 */
		        "\x41\x42\x43\x44"     /*  38 */
		"\x07\x10\x39\x38\x37\x36"     /*  44 */
		"\x08\x10\x29\x28\x27\x26"     /*  50 */
		"\x09\x10\x19\x18\x17\x16"     /*  54 */
		"\x0a\0"                       /*  56 */
		""; n = 56;

	vm_t vm;
	rc = vm_reset(&vm);
	assert(rc == 0);

	rc = vm_prime(&vm, code, n);
	assert(rc == 0);

	rc = vm_exec(&vm);
	assert(rc == 0);

	return 0;
}

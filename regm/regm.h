#ifndef REGM_H
#define REGM_H
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vigor.h>

/*

   a BIT is a 0 or a 1
   a NYBLE is 4 bits
   a BYTE is 8 bits
   a WORD is 2 BYTES, 16 BITS
   a DWORD (double word) is 4 BYTES, 32 BITS

   opcode format:

   15                7                 0
   | . . . . . . . . | . . . . . . . . |
   +-----------------+-----------------+
   | r0 |     op     |   t1   |   t2   |
   +-----------------+-----------------+
   |        optional operand1          |
   |               ...                 |
   +-----------------------------------+
   |        optional operand2          |
   |               ...                 |
   +-----------------------------------+

   r0     [2 bits]  RESERVED; must be 00
   op     [6 bits]  opcode value (0-63)
   t1     [4 bits]  type for operand 1
   t2     [4 bits]  type for operand 2
   operands are 32-bit DWORDs

 */

typedef uint8_t   byte_t;
typedef uint16_t  word_t;
typedef uint32_t dword_t;

typedef struct {
	dword_t val[254];
	byte_t  top;
} stack_t;

typedef struct {
	dword_t  addr;
	byte_t  *data;
	size_t   size;
	list_t   l;
} heap_t;

#define NREGS 16
typedef struct {
	dword_t   r[16];  /* generic registers */
	dword_t   acc;    /* accumulator register */
	dword_t   pc;     /* program counter register */

	stack_t  dstack; /* data stack */
	stack_t  istack; /* instruction stack */

	/* auxiliary */
	struct stat   stat;

	list_t   heap;
	dword_t  heaptop;

	size_t   codesize;
	byte_t  *code;
} vm_t;

#define HI_NYBLE(_) (((_) >> 4) & 0x0f)
#define LO_NYBLE(_) ( (_)       & 0x0f)
#define HI_BYTE(_)  (((_) >> 8) & 0xff);
#define LO_BYTE(_)  ( (_)       & 0xff);
#define WORD(a,b) ((a << 8) | (b))
#define DWORD(a,b,c,d) ((a << 24) | (b << 16) | (c << 8) | (d))

int vm_reset(vm_t *vm);
int vm_prime(vm_t *vm, byte_t *code, size_t len);
int vm_args(vm_t *vm, int argc, char **argv);
int vm_exec(vm_t *vm);

int push(stack_t *st, dword_t value);
dword_t pop(stack_t *st);
int empty(stack_t *st);

#endif

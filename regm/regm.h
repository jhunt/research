#ifndef REGM_H
#define REGM_H
#include <stdint.h>
#include <sys/types.h>

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
	word_t  val[254];
	byte_t  top;
} stack_t;

#define NREGS 16
typedef struct {
	word_t   r[16];  /* generic registers */
	word_t   acc;    /* accumulator register */
	word_t   pc;     /* program counter register */

	stack_t  dstack; /* data stack */
	stack_t  istack; /* instruction stack */

	size_t   codesize;
	byte_t  *code;

	size_t   heapsize;
	word_t  *heap;
} vm_t;

int vm_reset(vm_t *vm);
int vm_prime(vm_t *vm, byte_t *code, size_t len);
int vm_exec(vm_t *vm);

#endif

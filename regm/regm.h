#ifndef REGM_H
#define REGM_H
#include <stdint.h>

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
   | r0 |     op     |   f1   |   f2   |
   +-----------------+-----------------+
   |        optional operand1          |
   |               ...                 |
   +-----------------------------------+
   |        optional operand2          |
   |               ...                 |
   +-----------------------------------+

   r0     [2 bits]  RESERVED; must be 00
   op     [6 bits]  opcode value (0-63)
   f1     [4 bits]  flags for operand 1
   f2     [4 bits]  flags for operand 2
   operands are 32-bit DWORDs


   operand flag format (f1/f2):

   3         0
   | . . . . |
   +---------+
   | x <--------- 3: RESERVED; must be 0
   |   x <------- 2: operand type:
   |     x <----- 1:   00 = literal integer
   |                   01 = memory address
   |                   02 = register index
   |                   03 = RESERVED
   |       x <--- 0: present (1) or absent (0)
   +---------+

 */

typedef uint8_t   byte_t;
typedef uint16_t  word_t;
typedef uint32_t dword_t;

typedef struct {
	word_t  val[254];
	byte_t  top;
} stack_t;

typedef struct {
	word_t   reg[8]; /* generic registers */
	word_t   acc;    /* accumulator register */
	word_t   pc;     /* program counter register */

	stack_t  dstack; /* data stack */
	stack_t  istack; /* instruction stack */

	byte_t  *code;
	size_t   codesize;
} vm_t;

int vm_reset(vm_t *vm);
int vm_prime(vm_t *vm, byte_t *code, size_t len);
int vm_exec(vm_t *vm);

#endif

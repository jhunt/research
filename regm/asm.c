#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include <vigor.h>
#include "regm.h"

#define OPCODES_EXTENDED
#include "opcodes.h"

static const char * VTYPES[] = {
	"NONE",
	"register",
	"number",
	"string",
	"address",
	"label",
	"offset",
	"fnlabel",
	NULL,
};

#define VALUE_NONE      0x0
#define VALUE_REGISTER  0x1
#define VALUE_NUMBER    0x2
#define VALUE_STRING    0x3
#define VALUE_ADDRESS   0x4
#define VALUE_LABEL     0x5
#define VALUE_FNLABEL   0x6
#define VALUE_OFFSET    0x7

#define TYPE_LITERAL   0x1
#define TYPE_REGISTER  0x2
#define TYPE_ADDRESS   0x3

typedef struct {
	byte_t type;
	byte_t bintype;
	union {
		char     regname;   /* register (a, c, etc.) */
		dword_t  literal;   /* literal value         */
		char    *string;    /* string value          */
		dword_t  address;   /* memory address        */
		char    *label;     /* for labelled jumps    */
		char    *fnlabel;   /* for labelled jumps    */
		dword_t  offset;    /* for relative jumps    */
	} _;
} value_t;

#define SPECIAL_LABEL 1
#define SPECIAL_FUNC  2
typedef struct {
	byte_t  special;     /* identify special, non-code markers */
	char   *label;       /* special label */
	void   *fn;          /* link to the FN that starts scope */

	byte_t  op;          /* opcode number, for final encoding */
	value_t oper1;       /* first operand (optional) */
	value_t oper2;       /* second operand (optional) */

	dword_t offset;      /* byte offset in opcode binary stream */
	list_t  l;
} op_t;

typedef struct {
	const char *name;    /* name of the flag (for source correlation) */
	dword_t     idx;     /* index into heap array (address) */
	list_t      l;
} flag_t;

typedef struct {
	byte_t *mem;
	size_t  total;
	size_t  used;
	size_t  burst;
	dword_t offset;
} static_t;

list_t OPS;
static_t STATIC;
byte_t *CODE;


#define LINE_BUF_SIZE 8192
typedef struct {
	FILE       *io;
	const char *file;
	int         line;
	int         token;
	char        value[LINE_BUF_SIZE];
	char        buffer[LINE_BUF_SIZE];
	char        raw[LINE_BUF_SIZE];
} parser_t;

#define T_REGISTER        0x01
#define T_LABEL           0x02
#define T_IDENTIFIER      0x03
#define T_OFFSET          0x04
#define T_NUMBER          0x05
#define T_STRING          0x06
#define T_OPCODE          0x07
#define T_FUNCTION        0x08
#define T_TOPIC           0x09

static const char * T_names[] = {
	"(NONE)",
	"T_REGISTER",
	"T_LABEL",
	"T_IDENTIFIER",
	"T_OFFSET",
	"T_NUMBER",
	"T_STRING",
	"T_OPCODE",
	"T_FUNCTION",
	"T_TOPIC",
	NULL,
};

static int lex(parser_t *p)
{
	char *a, *b;
	if (!*p->buffer || *p->buffer == ';') {
getline:
		if (!fgets(p->raw, LINE_BUF_SIZE, p->io))
			return 0;
		p->line++;

		a = p->raw;
		while (*a && isspace(*a)) a++;
		if (*a == ';')
			while (*a && *a != '\n') a++;
		while (*a && isspace(*a)) a++;
		if (!*a)
			goto getline;

		b = p->buffer;
		while ((*b++ = *a++));
	}
	p->token = 0;
	p->value[0] = '\0';

	b = p->buffer;
	while (*b && isspace(*b)) b++;
	a = b;

	if (*b == '%') { /* register */
		while (!isspace(*b)) b++;
		if (!*b || isspace(*b)) {
			*b = '\0';
			char reg = 0;
			if (b - a == 2)
				reg = *(a + 1);
			if (reg < 'a' || reg >= 'a'+NREGS) {
				logger(LOG_ERR, "%s:%i: unrecognized register address %s (%i)", p->file, p->line, a, reg);
				return 0;
			}

			p->token = T_REGISTER;
			p->value[0] = reg;
			p->value[1] = '\0';

			b++; while (*b && isspace(*b)) b++;
			memmove(p->buffer, b, strlen(b)+1);
			return 1;
		}
		b = a;
	}

	if (*b == '+' || *b == '-') {
		b++;
		while (*b && isdigit(*b)) b++;
		if (!*b || isspace(*b)) {
			*b++ = '\0';

			p->token = T_OFFSET;
			memcpy(p->value, p->buffer, b-p->buffer);

			while (*b && isspace(*b)) b++;
			memmove(p->buffer, b, strlen(b)+1);
			return 1;
		}
		a = b;
	}

	if (isdigit(*b)) {
		while (*b && isdigit(*b)) b++;
		if (!*b || isspace(*b)) {
			*b++ = '\0';

			p->token = T_NUMBER;
			memcpy(p->value, p->buffer, b-p->buffer);

			while (*b && isspace(*b)) b++;
			memmove(p->buffer, b, strlen(b)+1);
			return 1;
		}
		a = b;
	}

	if (isalpha(*b)) {
		while (*b && !isspace(*b) && *b != ':')
			b++;
		if (*b == ':') {
			*b++ = '\0';
			p->token = T_LABEL;
			memcpy(p->value, p->buffer, b-p->buffer);
			while (*b && isspace(*b)) b++;
			memmove(p->buffer, b, strlen(b)+1);
			return 1;
		}
		b = a;

		while (*b && (isalnum(*b) || *b == '.' || *b == '_' || *b == '?'))
			b++;
		*b++ = '\0';

		int i;
		for (i = 0; ASM[i]; i++) {
			if (strcmp(a, ASM[i]) != 0) continue;
			p->token = T_OPCODE;
			p->value[0] = T_OPCODE_NOOP + i;
			p->value[1] = '\0';
			break;
		}

		if (strcmp(a, "fn") == 0) p->token = T_FUNCTION;

		if (!p->token) {
			p->token = T_IDENTIFIER;
			memcpy(p->value, p->buffer, b-p->buffer);
		}

		while (*b && isspace(*b)) b++;
		memmove(p->buffer, b, strlen(b)+1);
		return 1;
	}

	if (*b == '"') {
		b++; a = p->value;
		while (*b && *b != '"' && *b != '\r' && *b != '\n') {
			if (*b == '\\') b++;
			*a++ = *b++;
		}
		*a = '\0';
		if (*b == '"') b++;
		else logger(LOG_WARNING, "%s:%i: unterminated string literal", p->file, p->line);

		p->token = T_STRING;
		while (*b && isspace(*b)) b++;
		memmove(p->buffer, b, strlen(b)+1);
		return 1;
	}

	if (*b == '@') {
		int n = 0;
		while (*b == '@') { n++; b++; }
		if (n > 2) {
			p->token = T_TOPIC;
			while (*b && isspace(*b)) b++;
			a = b;
			while (*b && !isspace(*b)) b++;
			*b++ = '\0';
			memcpy(p->value, a, b-a);

			while (*b && isspace(*b)) b++;
			memmove(p->buffer, b, strlen(b)+1);
			return 1;
		}
		b = a;
	}


	logger(LOG_ERR, "%s:%i: failed to parse '%s'", p->file, p->line, p->buffer);
	return 0;
}

static int parse(void)
{
	parser_t p;
	memset(&p, 0, sizeof(p));
	p.file = strdup("<stdin>");
	p.io   = stdin;

	op_t *FN = NULL;
	list_init(&OPS);

#define NEXT if (!lex(&p)) { logger(LOG_CRIT, "%s:%i: unexpected end of configuration\n", p.file, p.line); goto bail; }
#define ERROR(s) do { logger(LOG_CRIT, "%s:%i: syntax error: %s", p.file, p.line, s); goto bail; } while (0)

#define OPERAND_REGISTER(x) do { op->x.type = VALUE_REGISTER; op->x._.regname = p.value[0];      } while (0)
#define OPERAND_NUMBER(x)   do { op->x.type = VALUE_NUMBER;   op->x._.literal = atoi(p.value);   } while (0)
#define OPERAND_STRING(x)   do { op->x.type = VALUE_STRING;   op->x._.string  = strdup(p.value); } while (0)
#define OPERAND_LABEL(x)    do { op->x.type = VALUE_LABEL;    op->x._.label   = strdup(p.value); } while (0)
#define OPERAND_FNLABEL(x)  do { op->x.type = VALUE_FNLABEL;  op->x._.fnlabel = strdup(p.value); } while (0)
#define OPERAND_OFFSET(x)   do { op->x.type = VALUE_OFFSET;   op->x._.offset  = atoi(p.value);   } while (0)

	while (lex(&p)) {

		if (!p.value[0])
			fprintf(stderr, "%02x %-14s\n", p.token, T_names[p.token]);
		else if (p.token == T_OPCODE)
			fprintf(stderr, "%02x %-14s : %#04x (%s)\n", p.token, T_names[p.token],
				p.value[0] & 0xff, ASM[(p.value[0] & 0xff) - T_OPCODE_NOOP]);
		else
			fprintf(stderr, "%02x %-14s : %s\n", p.token, T_names[p.token], p.value);

		op_t *op = calloc(1, sizeof(op_t));
		op->fn = FN;

		switch (p.token) {
		case T_TOPIC:
			break;

		case T_FUNCTION:
			FN = op;
			op->special = SPECIAL_FUNC;
			NEXT;
			if (p.token != T_IDENTIFIER)
				ERROR("unacceptable name for function");
			op->label = strdup(p.value);
			break;

		case T_LABEL:
			op->special = SPECIAL_LABEL;
			op->label = strdup(p.value);
			break;

		case T_OPCODE:
			switch (p.value[0]) {
			case T_OPCODE_PUSH:
				op->op = PUSH;
				NEXT;
				if (p.token != T_REGISTER)
					ERROR("non-register operand given to push opcode");
				op->oper1.type = VALUE_REGISTER;
				op->oper1._.regname = p.value[0];
				break;

			case T_OPCODE_POP:
				op->op = POP;
				NEXT;
				if (p.token != T_REGISTER)
					ERROR("non-register operand given to pop opcode");
				op->oper1.type = VALUE_REGISTER;
				op->oper1._.regname = p.value[0];
				break;

			case T_OPCODE_SET:
				op->op = SET;
				NEXT;
				if (p.token != T_REGISTER)
					ERROR("non-register operand given to set opcode");
				OPERAND_REGISTER(oper1);

				NEXT;
				switch (p.token) {
				case T_REGISTER: OPERAND_REGISTER(oper2); break;
				case T_NUMBER:   OPERAND_NUMBER(oper2);   break;
				case T_STRING:   OPERAND_STRING(oper2);   break;
				default:
					ERROR("unrecognized value operand given to set opcode");
				}
				break;

			case T_OPCODE_SWAP:
				op->op = SWAP;
				NEXT;
				if (p.token != T_REGISTER)
					ERROR("non-register operand given to swap opcode");
				OPERAND_REGISTER(oper1);

				NEXT;
				if (p.token != T_REGISTER)
					ERROR("non-register operand given to swap opcode");
				OPERAND_REGISTER(oper2);
				break;

			case T_OPCODE_CALL:
				op->op = CALL;
				NEXT;
				if (p.token != T_IDENTIFIER)
					ERROR("non-literal function reference given to call opcode");
				OPERAND_FNLABEL(oper1);
				break;

			case T_OPCODE_RET:
				op->op = RET;
				break;

			case T_OPCODE_RETV:
				op->op = RET;
				NEXT;
				switch (p.token) {
				case T_REGISTER: OPERAND_REGISTER(oper1); break;
				case T_NUMBER:   OPERAND_NUMBER(oper1);   break;
				case T_STRING:   OPERAND_STRING(oper1);   break;
				default:
					ERROR("unrecognized value operand given to ret opcode");
				}
				break;

			case T_OPCODE_CMP:
				op->op = CMP;
				NEXT;
				switch (p.token) {
				case T_REGISTER: OPERAND_REGISTER(oper1); break;
				case T_NUMBER:   OPERAND_NUMBER(oper1);   break;
				case T_STRING:
					ERROR("string given as operand 1 of cmp opcode");
				default:
					ERROR("unrecognized value operand given to ret opcode");
				}

				NEXT;
				switch (p.token) {
				case T_REGISTER: OPERAND_REGISTER(oper2); break;
				case T_NUMBER:   OPERAND_NUMBER(oper2);   break;
				case T_STRING:
					ERROR("string given as operand 2 of cmp opcode");
				default:
					ERROR("unrecognized value operand given to cmp opcode");
				}
				break;

			case T_OPCODE_STRCMP:
				op->op = STRCMP;
				NEXT;
				switch (p.token) {
				case T_REGISTER: OPERAND_REGISTER(oper1); break;
				case T_STRING:   OPERAND_STRING(oper1);   break;
				case T_NUMBER:
					ERROR("non-string given as operand 1 of strcmp opcode");
				default:
					ERROR("unrecognized value operand given to strcmp opcode");
				}
				NEXT;
				switch (p.token) {
				case T_REGISTER: OPERAND_REGISTER(oper2); break;
				case T_STRING:   OPERAND_STRING(oper2);   break;
				case T_NUMBER:
					ERROR("non-string given as operand 2 of strcmp opcode");
				default:
					ERROR("unrecognized value operand given to strcmp opcode");
				}
				break;

			case T_OPCODE_JMP:
				op->op = JMP;
				NEXT;
				switch (p.token) {
				case T_IDENTIFIER: OPERAND_LABEL(oper1);  break;
				case T_OFFSET:     OPERAND_OFFSET(oper1); break;
				default:
					ERROR("non-label operand given to jmp opcode");
				}
				break;

			case T_OPCODE_JZ:
				op->op = JZ;
				NEXT;
				switch (p.token) {
				case T_IDENTIFIER: OPERAND_LABEL(oper1);  break;
				case T_OFFSET:     OPERAND_OFFSET(oper1); break;
				default:
					ERROR("non-label operand given to jz opcode");
				}
				break;

			case T_OPCODE_JNZ:
				op->op = JNZ;
				NEXT;
				switch (p.token) {
				case T_IDENTIFIER: OPERAND_LABEL(oper1);  break;
				case T_OFFSET:     OPERAND_OFFSET(oper1); break;
				default:
					ERROR("non-label operand given to jnz opcode");
				}
				break;

			case T_OPCODE_HALT:
				op->op = HALT;
				break;

			case T_OPCODE_DUMP:
				op->op = DUMP;
				break;

			case T_OPCODE_ERR:
				op->op = ERR;
				NEXT;
				if (p.token != T_STRING)
					ERROR("non-string argument given to err opcode");
				OPERAND_STRING(oper1);
				break;

			case T_OPCODE_PERROR:
				op->op = PERROR;
				NEXT;
				if (p.token != T_STRING)
					ERROR("non-string argument given to perror opcode");
				OPERAND_STRING(oper1);
				break;

			case T_OPCODE_BAIL:
				op->op = BAIL;
				break;

			case T_OPCODE_FSTAT:
				op->op = FSTAT;
				NEXT;
				break;

			case T_OPCODE_ISFILE:
				op->op = ISFILE;
				NEXT;
				break;

			case T_OPCODE_ISLINK:
				op->op = ISLINK;
				NEXT;
				break;

			case T_OPCODE_ISDIR:
				op->op = ISDIR;
				NEXT;
				break;

			case T_OPCODE_TOUCH:
				op->op = TOUCH;
				NEXT;
				break;

			case T_OPCODE_UNLINK:
				op->op = UNLINK;
				NEXT;
				break;

			case T_OPCODE_RENAME:
				op->op = RENAME;
				NEXT;
				NEXT;
				break;

			case T_OPCODE_CHOWN:
				op->op = CHOWN;
				NEXT;
				NEXT;
				break;

			case T_OPCODE_CHGRP:
				op->op = CHGRP;
				NEXT;
				NEXT;
				break;

			case T_OPCODE_CHMOD:
				op->op = CHMOD;
				NEXT;
				NEXT;
				break;

			case T_OPCODE_FSHA1:
				op->op = FSHA1;
				NEXT;
				NEXT;
				break;

			case T_OPCODE_GETFILE:
				op->op = GETFILE;
				NEXT;
				NEXT;
				break;

			case T_OPCODE_GETUID:
				op->op = GETUID;
				NEXT;
				NEXT;
				break;

			case T_OPCODE_GETGID:
				op->op = GETGID;
				NEXT;
				NEXT;
				break;

			case T_OPCODE_EXEC:
				op->op = EXEC;
				NEXT;
				NEXT;
				break;

			}
			break;

		case T_REGISTER:   ERROR("unexpected register reference found at top-level");
		case T_IDENTIFIER: ERROR("unexpected identifier found at top-level");
		case T_OFFSET:     ERROR("unexpected offset found at top-level");
		case T_NUMBER:     ERROR("unexpected numeric literal found at top-level");
		case T_STRING:     ERROR("unexpected string literal found at top-level");

		default:
			ERROR("unhandled token type");
		}

		list_push(&OPS, &op->l);
	}

	return 0;

bail:
	return 1;
}

hash_t strings;
static int s_resolve(value_t *v, op_t *me)
{
	byte_t *addr;
	size_t len;
	op_t *op, *fn;

	switch (v->type) {
	case VALUE_REGISTER:
		v->bintype = TYPE_REGISTER;
		v->_.literal -= 'a';
		return 0;

	case VALUE_NUMBER:
		v->bintype = TYPE_LITERAL;
		return 0;

	case VALUE_STRING:
		addr = hash_get(&strings, v->_.string);
		if (!addr) {
			len = strlen(v->_.string) + 1;
			if (STATIC.total - STATIC.used < len) {
				/* FIXME: only works for strings < STATIC.burst */
				byte_t *mem = realloc(STATIC.mem, STATIC.total + STATIC.burst);
				if (!mem) perror("realloc");

				STATIC.mem = mem;
				STATIC.total += STATIC.burst;
			}
			memcpy(STATIC.mem + STATIC.used, v->_.string, len);
			fprintf(stderr, "s_resolve: relocated string %s to %#010x\n",
				v->_.string, (unsigned)(STATIC.offset + STATIC.used));

			addr = STATIC.mem + STATIC.used;
			hash_set(&strings, v->_.string, addr);
			STATIC.used += len;
		}

		free(v->_.string);

		v->type = VALUE_ADDRESS;
		v->bintype = TYPE_ADDRESS;
		v->_.address = STATIC.offset + (addr - STATIC.mem);
		return 0;

	case VALUE_ADDRESS:
		v->bintype = TYPE_ADDRESS;
		return 0;

	case VALUE_LABEL:
		fn = (op_t*)me->fn;
		for_each_object(op, &fn->l, l) {
			if (op->special == SPECIAL_FUNC) break;
			if (op->special != SPECIAL_LABEL) continue;
			if (strcmp(v->_.label, op->label) != 0) continue;

			free(v->_.label);
			v->type = VALUE_ADDRESS;
			v->bintype = TYPE_ADDRESS;
			v->_.address = op->offset;
			return 0;
		}
		logger(LOG_ERR, "label %s not found in scope!", v->_.label);
		return 1;

	case VALUE_FNLABEL:
		for_each_object(op, &me->l, l) {
			if (op->special != SPECIAL_FUNC) continue;
			if (strcmp(v->_.fnlabel, op->label) != 0) continue;

			free(v->_.fnlabel);
			v->type = VALUE_ADDRESS;
			v->bintype = TYPE_ADDRESS;
			v->_.address = op->offset;
			return 0;
		}
		logger(LOG_ERR, "fnlabel %s not found globally!", v->_.fnlabel);
		return 1;

	case VALUE_OFFSET:
		for_each_object(op, &me->l, l) {
			if (op->special) continue;
			if (v->_.offset--) continue;

			v->type = VALUE_ADDRESS;
			v->bintype = TYPE_ADDRESS;
			v->_.address = op->offset;
			return 0;
		}
		return 1;
	}
	return 0;
}

static int compile(void)
{
	/* phases of compilation:

	   I.   determine offset of each opcode
	   II.  resolve labels / relative addresses
	   III. pack 'external memory' data
	   IV.  encode
	 */

	memset(&STATIC, 0, sizeof(STATIC));
	STATIC.burst = 256 * 1024;

	op_t *op;
	int rc;

	/* sneakily insert a HALT instruction at the end */
	op = calloc(1, sizeof(op_t));
	op->op = HALT;
	list_push(&OPS, &op->l);

	/* phase I: calculate offsets */
	dword_t offset = 0;
	for_each_object(op, &OPS, l) {
		op->offset = offset;
		if (op->special) continue;
		offset += 2;                                   /* 2-byte opcode  */
		if (op->oper1.type != VALUE_NONE) offset += 4; /* 4-byte operand */
		if (op->oper2.type != VALUE_NONE) offset += 4; /* 4-byte operand */
	}
	STATIC.offset = offset;
	CODE = calloc(offset, sizeof(byte_t));
	byte_t *c = CODE;

	for_each_object(op, &OPS, l) {
		if (op->special) continue;

		/* phase II/III: resolve labels / pack strings */
		rc = s_resolve(&op->oper1, op);
		assert(rc == 0);
		rc = s_resolve(&op->oper2, op);
		assert(rc == 0);

		/* phase IV: encode */
		*c++ = op->op;
		*c++ = ((op->oper1.bintype & 0xff) << 4)
		     | ((op->oper2.bintype & 0xff));

		if (op->oper1.type) {
			*c++ = ((op->oper1._.literal >> 24) & 0xff);
			*c++ = ((op->oper1._.literal >> 16) & 0xff);
			*c++ = ((op->oper1._.literal >>  8) & 0xff);
			*c++ = ((op->oper1._.literal >>  0) & 0xff);
		}
		if (op->oper2.type) {
			*c++ = ((op->oper2._.literal >> 24) & 0xff);
			*c++ = ((op->oper2._.literal >> 16) & 0xff);
			*c++ = ((op->oper2._.literal >>  8) & 0xff);
			*c++ = ((op->oper2._.literal >>  0) & 0xff);
		}
	}

	return 0;
}

int main(int argc, char **argv)
{
	log_open("asm", "console");

	int rc = parse();
	assert(rc == 0);

	rc = compile();
	assert(rc == 0);

	op_t *op;
	for_each_object(op, &OPS, l) {
		fprintf(stderr, "%08x [%04x] %s\n", op->offset, op->op, OPCODES[op->op]);
		if (op->oper1.type != VALUE_NONE) {
			fprintf(stderr, "%8s 1: %s (%i)\n", " ", VTYPES[op->oper1.type], op->oper1.type);
			switch (op->oper1.type) {
			case VALUE_NUMBER:  fprintf(stderr, "%11s= %i\n", " ", op->oper1._.literal); break;
			case VALUE_LABEL:   fprintf(stderr, "%11s @%s\n", " ", op->oper1._.label); break;
			case VALUE_STRING:  fprintf(stderr, "%11s \"%s\"\n", " ", op->oper1._.string); break;
			case VALUE_ADDRESS: fprintf(stderr, "%11s %#010x\n", " ", op->oper1._.address); break;
			}
		}
		if (op->oper2.type != VALUE_NONE) {
			fprintf(stderr, "%8s 2: %s (%i)\n", " ", VTYPES[op->oper2.type], op->oper2.type);
			switch (op->oper2.type) {
			case VALUE_NUMBER:  fprintf(stderr, "%11s= %i\n", " ", op->oper2._.literal); break;
			case VALUE_LABEL:   fprintf(stderr, "%11s @%s\n", " ", op->oper2._.label); break;
			case VALUE_STRING:  fprintf(stderr, "%11s \"%s\"\n", " ", op->oper2._.string); break;
			case VALUE_ADDRESS: fprintf(stderr, "%11s %0#10x\n", " ", op->oper2._.address); break;
			}
		}
		fprintf(stderr, "\n");
	}

	write(1, CODE, STATIC.offset);
	write(1, STATIC.mem, STATIC.used);

	return 0;
}

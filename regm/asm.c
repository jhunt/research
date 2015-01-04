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
	value_t args[2];     /* operands */

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
		if (*b == '0' && *(b+1) == 'x') {
			b += 2;
			while (*b && isxdigit(*b)) b++;
		} else {
			while (*b && isdigit(*b)) b++;
		}
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
			if (*b == '\\') {
				b++;
				switch (*b) {
				case 'n': *a = '\n'; break;
				case 'r': *a = '\r'; break;
				case 't': *a = '\t'; break;
				default:  *a = *b;
				}
				a++; b++;
			} else *a++ = *b++;
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

	int i, j;
	op_t *op;
	while (lex(&p)) {

		if (!p.value[0])
			fprintf(stderr, "%02x %-14s\n", p.token, T_names[p.token]);
		else if (p.token == T_OPCODE)
			fprintf(stderr, "%02x %-14s : %#04x (%s)\n", p.token, T_names[p.token],
				p.value[0] & 0xff, ASM[(p.value[0] & 0xff) - T_OPCODE_NOOP]);
		else
			fprintf(stderr, "%02x %-14s : %s\n", p.token, T_names[p.token], p.value);

		op = calloc(1, sizeof(op_t));
		op->fn = FN;

		switch (p.token) {
		case T_TOPIC:
			break;

		case T_FUNCTION:
			if (FN && list_tail(&OPS, op_t, l)->op != RET) {
				op->op = RET;
				list_push(&OPS, &op->l);
				op = calloc(1, sizeof(op_t));
			}
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
			for (i = 0; ASM_SYNTAX[i].token; i++) {
				if (p.value[0] != ASM_SYNTAX[i].token) continue;
				op->op = ASM_SYNTAX[i].opcode;

				for (j = 0; j < 2; j++) {
					if (ASM_SYNTAX[i].args[j] == ARG_NONE) break;
					NEXT;

					if (p.token == T_REGISTER && ASM_SYNTAX[i].args[j] & ARG_REGISTER) {
						op->args[j].type = VALUE_REGISTER;
						op->args[j]._.regname = p.value[0];

					} else if (p.token == T_NUMBER && ASM_SYNTAX[i].args[j] & ARG_NUMBER) {
						op->args[j].type = VALUE_NUMBER;
						op->args[j]._.literal = strtol(p.value, NULL, 0);

					} else if (p.token == T_STRING && ASM_SYNTAX[i].args[j] & ARG_STRING) {
						op->args[j].type = VALUE_STRING;
						op->args[j]._.string = strdup(p.value);

					} else if (p.token == T_IDENTIFIER && ASM_SYNTAX[i].args[j] & ARG_LABEL) {
						op->args[j].type = VALUE_LABEL;
						op->args[j]._.label = strdup(p.value);

					} else if (p.token == T_IDENTIFIER && ASM_SYNTAX[i].args[j] & ARG_FUNCTION) {
						op->args[j].type = VALUE_FNLABEL;
						op->args[j]._.fnlabel = strdup(p.value);

					} else if (p.token == T_OFFSET && ASM_SYNTAX[i].args[j] & ARG_LABEL) {
						op->args[j].type = VALUE_OFFSET;
						op->args[j]._.offset = strtol(p.value, NULL, 10);

					} else {
						logger(LOG_CRIT, "%s: %i: invalid form; expected `%s`",
							p.file, p.line, ASM_SYNTAX[i].usage);
						goto bail;
					}
				}
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

	if (FN && list_tail(&OPS, op_t, l)->op != RET) {
		op = calloc(1, sizeof(op_t));
		op->op = RET;
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

	   I.   insert runtime at addr 0
	   II.  determine offset of each opcode
	   III. resolve labels / relative addresses
	   IV.  pack 'external memory' data
	   V.   encode
	 */

	memset(&STATIC, 0, sizeof(STATIC));
	STATIC.burst = 256 * 1024;

	op_t *op;
	int rc;

	/* phase I: runtime insertion */
	op = calloc(1, sizeof(op_t));
	op->op = JMP; /* JMP, don't CALL */
	op->args[0].type = VALUE_FNLABEL;
	op->args[0]._.label = strdup("main");
	list_unshift(&OPS, &op->l);

	/* phase II: calculate offsets */
	dword_t offset = 0;
	for_each_object(op, &OPS, l) {
		op->offset = offset;
		if (op->special) continue;
		offset += 2;                                   /* 2-byte opcode  */
		if (op->args[0].type != VALUE_NONE) offset += 4; /* 4-byte operand */
		if (op->args[1].type != VALUE_NONE) offset += 4; /* 4-byte operand */
	}
	STATIC.offset = offset;
	CODE = calloc(offset, sizeof(byte_t));
	byte_t *c = CODE;

	for_each_object(op, &OPS, l) {
		if (op->special) continue;

		/* phase II/III: resolve labels / pack strings */
		rc = s_resolve(&op->args[0], op);
		assert(rc == 0);
		rc = s_resolve(&op->args[1], op);
		assert(rc == 0);

		/* phase IV: encode */
		*c++ = op->op;
		*c++ = ((op->args[0].bintype & 0xff) << 4)
		     | ((op->args[1].bintype & 0xff));

		if (op->args[0].type) {
			*c++ = ((op->args[0]._.literal >> 24) & 0xff);
			*c++ = ((op->args[0]._.literal >> 16) & 0xff);
			*c++ = ((op->args[0]._.literal >>  8) & 0xff);
			*c++ = ((op->args[0]._.literal >>  0) & 0xff);
		}
		if (op->args[1].type) {
			*c++ = ((op->args[1]._.literal >> 24) & 0xff);
			*c++ = ((op->args[1]._.literal >> 16) & 0xff);
			*c++ = ((op->args[1]._.literal >>  8) & 0xff);
			*c++ = ((op->args[1]._.literal >>  0) & 0xff);
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
		if (op->args[0].type != VALUE_NONE) {
			fprintf(stderr, "%8s 1: %s (%i)\n", " ", VTYPES[op->args[0].type], op->args[0].type);
			switch (op->args[0].type) {
			case VALUE_NUMBER:  fprintf(stderr, "%11s= %i\n", " ", op->args[0]._.literal); break;
			case VALUE_LABEL:   fprintf(stderr, "%11s @%s\n", " ", op->args[0]._.label); break;
			case VALUE_STRING:  fprintf(stderr, "%11s \"%s\"\n", " ", op->args[0]._.string); break;
			case VALUE_ADDRESS: fprintf(stderr, "%11s %#010x\n", " ", op->args[0]._.address); break;
			}
		}
		if (op->args[1].type != VALUE_NONE) {
			fprintf(stderr, "%8s 2: %s (%i)\n", " ", VTYPES[op->args[1].type], op->args[1].type);
			switch (op->args[1].type) {
			case VALUE_NUMBER:  fprintf(stderr, "%11s= %i\n", " ", op->args[1]._.literal); break;
			case VALUE_LABEL:   fprintf(stderr, "%11s @%s\n", " ", op->args[1]._.label); break;
			case VALUE_STRING:  fprintf(stderr, "%11s \"%s\"\n", " ", op->args[1]._.string); break;
			case VALUE_ADDRESS: fprintf(stderr, "%11s %0#10x\n", " ", op->args[1]._.address); break;
			}
		}
		fprintf(stderr, "\n");
	}

	write(1, CODE, STATIC.offset);
	write(1, STATIC.mem, STATIC.used);

	return 0;
}

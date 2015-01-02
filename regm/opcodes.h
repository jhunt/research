/** OPCODE CONSTANTS **/
#define NOOP     0000  /* does nothing */
#define PUSH     0x01  /* push a register onto data stack */
#define POP      0x02  /* pop data stack top into a register */
#define SET      0x03  /* set register value */
#define SWAP     0x04  /* swap two register values */
#define CALL     0x05  /* call a user-defined function */
#define RET      0x06  /* return to caller */
#define CMP      0x07  /* compare two integers */
#define STRCMP   0x08  /* compare two strings */
#define JMP      0x09  /* unconditional jump */
#define JZ       0x0a  /* jump if accumulator is 0 */
#define JNZ      0x0b  /* jump if accumulator is not 0 */
#define ERR      0x0c  /* print an error */
#define PERROR   0x0d  /* print an error (with system error message) */
#define BAIL     0x0e  /* ... */
#define MARK     0x0f  /* ... */
#define FSTAT    0x10  /* check to see if a file exists */
#define ISFILE   0x11  /* is a path a regular file? */
#define ISLINK   0x12  /* is a path a symbolic link? */
#define ISDIR    0x13  /* is a path a directory? */
#define TOUCH    0x14  /* touch a file (create or update its utime) */
#define UNLINK   0x15  /* remove a file */
#define RENAME   0x16  /* rename a file */
#define CHOWN    0x17  /* change file ownership */
#define CHGRP    0x18  /* change file group ownership */
#define CHMOD    0x19  /* change file permissions */
#define FSHA1    0x1a  /* calculate SHA1 of a file's contents */
#define GETFILE  0x1b  /* retrieve a file from the server */
#define GETUID   0x1c  /* look up a user's UID, by name */
#define GETGID   0x1d  /* look up a group's GID, by name */
#define EXEC     0x1e  /* execute a command */
#define DUMP     0x1f  /* dump virtual machine state for debugging */
#define HALT     0x20  /* halt the virtual machine */


#ifdef OPCODES_EXTENDED
/** OPCODE MNEMONIC NAMES **/
static const char * OPCODES[] = {
	"noop",    /* NOOP      0  0000 */
	"push",    /* PUSH      1  0x01 */
	"pop",     /* POP       2  0x02 */
	"set",     /* SET       3  0x03 */
	"swap",    /* SWAP      4  0x04 */
	"call",    /* CALL      5  0x05 */
	"ret",     /* RET       6  0x06 */
	"cmp",     /* CMP       7  0x07 */
	"strcmp",  /* STRCMP    8  0x08 */
	"jmp",     /* JMP       9  0x09 */
	"jz",      /* JZ       10  0x0a */
	"jnz",     /* JNZ      11  0x0b */
	"err",     /* ERR      12  0x0c */
	"perror",  /* PERROR   13  0x0d */
	"bail",    /* BAIL     14  0x0e */
	"mark",    /* MARK     15  0x0f */
	"fstat",   /* FSTAT    16  0x10 */
	"isfile",  /* ISFILE   17  0x11 */
	"islink",  /* ISLINK   18  0x12 */
	"isdir",   /* ISDIR    19  0x13 */
	"touch",   /* TOUCH    20  0x14 */
	"unlink",  /* UNLINK   21  0x15 */
	"rename",  /* RENAME   22  0x16 */
	"chown",   /* CHOWN    23  0x17 */
	"chgrp",   /* CHGRP    24  0x18 */
	"chmod",   /* CHMOD    25  0x19 */
	"fsha1",   /* FSHA1    26  0x1a */
	"getfile", /* GETFILE  27  0x1b */
	"getuid",  /* GETUID   28  0x1c */
	"getgid",  /* GETGID   29  0x1d */
	"exec",    /* EXEC     30  0x1e */
	"dump",    /* DUMP     31  0x1f */
	"halt",    /* HALT     32  0x20 */
	NULL,
};


/** ASM TOKENS **/
#define T_OPCODE_NOOP     0x40  /* does nothing */
#define T_OPCODE_PUSH     0x41  /* push a register onto data stack */
#define T_OPCODE_POP      0x42  /* pop data stack top into a register */
#define T_OPCODE_SET      0x43  /* set register value */
#define T_OPCODE_SWAP     0x44  /* swap two register values */
#define T_OPCODE_CALL     0x45  /* call a user-defined function */
#define T_OPCODE_RET      0x46  /* return to caller */
#define T_OPCODE_RETV     0x47  /* return to caller (with value) */
#define T_OPCODE_CMP      0x48  /* compare two integers */
#define T_OPCODE_STRCMP   0x49  /* compare two strings */
#define T_OPCODE_JMP      0x4a  /* unconditional jump */
#define T_OPCODE_JZ       0x4b  /* jump if accumulator is 0 */
#define T_OPCODE_JNZ      0x4c  /* jump if accumulator is not 0 */
#define T_OPCODE_ERR      0x4d  /* print an error */
#define T_OPCODE_PERROR   0x4e  /* print an error (with system error message) */
#define T_OPCODE_BAIL     0x4f  /* ... */
#define T_OPCODE_MARK     0x50  /* ... */
#define T_OPCODE_FSTAT    0x51  /* check to see if a file exists */
#define T_OPCODE_ISFILE   0x52  /* is a path a regular file? */
#define T_OPCODE_ISLINK   0x53  /* is a path a symbolic link? */
#define T_OPCODE_ISDIR    0x54  /* is a path a directory? */
#define T_OPCODE_TOUCH    0x55  /* touch a file (create or update its utime) */
#define T_OPCODE_UNLINK   0x56  /* remove a file */
#define T_OPCODE_RENAME   0x57  /* rename a file */
#define T_OPCODE_CHOWN    0x58  /* change file ownership */
#define T_OPCODE_CHGRP    0x59  /* change file group ownership */
#define T_OPCODE_CHMOD    0x5a  /* change file permissions */
#define T_OPCODE_FSHA1    0x5b  /* calculate SHA1 of a file's contents */
#define T_OPCODE_GETFILE  0x5c  /* retrieve a file from the server */
#define T_OPCODE_GETUID   0x5d  /* look up a user's UID, by name */
#define T_OPCODE_GETGID   0x5e  /* look up a group's GID, by name */
#define T_OPCODE_EXEC     0x5f  /* execute a command */
#define T_OPCODE_DUMP     0x60  /* dump virtual machine state for debugging */
#define T_OPCODE_HALT     0x61  /* halt the virtual machine */


static const char * ASM[] = {
	"noop",    /* T_OPCODE_NOOP      0  0000 */
	"push",    /* T_OPCODE_PUSH      1  0x01 */
	"pop",     /* T_OPCODE_POP       2  0x02 */
	"set",     /* T_OPCODE_SET       3  0x03 */
	"swap",    /* T_OPCODE_SWAP      4  0x04 */
	"call",    /* T_OPCODE_CALL      5  0x05 */
	"ret",     /* T_OPCODE_RET       6  0x06 */
	"retv",    /* T_OPCODE_RETV      7  0x07 */
	"cmp",     /* T_OPCODE_CMP       8  0x08 */
	"strcmp",  /* T_OPCODE_STRCMP    9  0x09 */
	"jmp",     /* T_OPCODE_JMP      10  0x0a */
	"jz",      /* T_OPCODE_JZ       11  0x0b */
	"jnz",     /* T_OPCODE_JNZ      12  0x0c */
	"err",     /* T_OPCODE_ERR      13  0x0d */
	"perror",  /* T_OPCODE_PERROR   14  0x0e */
	"bail",    /* T_OPCODE_BAIL     15  0x0f */
	"mark",    /* T_OPCODE_MARK     16  0x10 */
	"fstat",   /* T_OPCODE_FSTAT    17  0x11 */
	"isfile",  /* T_OPCODE_ISFILE   18  0x12 */
	"islink",  /* T_OPCODE_ISLINK   19  0x13 */
	"isdir",   /* T_OPCODE_ISDIR    20  0x14 */
	"touch",   /* T_OPCODE_TOUCH    21  0x15 */
	"unlink",  /* T_OPCODE_UNLINK   22  0x16 */
	"rename",  /* T_OPCODE_RENAME   23  0x17 */
	"chown",   /* T_OPCODE_CHOWN    24  0x18 */
	"chgrp",   /* T_OPCODE_CHGRP    25  0x19 */
	"chmod",   /* T_OPCODE_CHMOD    26  0x1a */
	"fsha1",   /* T_OPCODE_FSHA1    27  0x1b */
	"getfile", /* T_OPCODE_GETFILE  28  0x1c */
	"getuid",  /* T_OPCODE_GETUID   29  0x1d */
	"getgid",  /* T_OPCODE_GETGID   30  0x1e */
	"exec",    /* T_OPCODE_EXEC     31  0x1f */
	"dump",    /* T_OPCODE_DUMP     32  0x20 */
	"halt",    /* T_OPCODE_HALT     33  0x21 */
	NULL,
};
#endif

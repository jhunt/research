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
#define ECHO     0x0c  /* print a message to standard output */
#define ERR      0x0d  /* print an error */
#define PERROR   0x0e  /* print an error (with system error message) */
#define BAIL     0x0f  /* ... */
#define MARK     0x10  /* ... */
#define FSTAT    0x11  /* check to see if a file exists */
#define ISFILE   0x12  /* is a path a regular file? */
#define ISLINK   0x13  /* is a path a symbolic link? */
#define ISDIR    0x14  /* is a path a directory? */
#define TOUCH    0x15  /* touch a file (create or update its utime) */
#define UNLINK   0x16  /* remove a file */
#define RENAME   0x17  /* rename a file */
#define CHOWN    0x18  /* change file ownership */
#define CHGRP    0x19  /* change file group ownership */
#define CHMOD    0x1a  /* change file permissions */
#define FSHA1    0x1b  /* calculate SHA1 of a file's contents */
#define GETFILE  0x1c  /* retrieve a file from the server */
#define GETUID   0x1d  /* look up a user's UID, by name */
#define GETGID   0x1e  /* look up a group's GID, by name */
#define EXEC     0x1f  /* execute a command */
#define DUMP     0x20  /* dump virtual machine state for debugging */
#define HALT     0x21  /* halt the virtual machine */


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
	"echo",    /* ECHO     12  0x0c */
	"err",     /* ERR      13  0x0d */
	"perror",  /* PERROR   14  0x0e */
	"bail",    /* BAIL     15  0x0f */
	"mark",    /* MARK     16  0x10 */
	"fstat",   /* FSTAT    17  0x11 */
	"isfile",  /* ISFILE   18  0x12 */
	"islink",  /* ISLINK   19  0x13 */
	"isdir",   /* ISDIR    20  0x14 */
	"touch",   /* TOUCH    21  0x15 */
	"unlink",  /* UNLINK   22  0x16 */
	"rename",  /* RENAME   23  0x17 */
	"chown",   /* CHOWN    24  0x18 */
	"chgrp",   /* CHGRP    25  0x19 */
	"chmod",   /* CHMOD    26  0x1a */
	"fsha1",   /* FSHA1    27  0x1b */
	"getfile", /* GETFILE  28  0x1c */
	"getuid",  /* GETUID   29  0x1d */
	"getgid",  /* GETGID   30  0x1e */
	"exec",    /* EXEC     31  0x1f */
	"dump",    /* DUMP     32  0x20 */
	"halt",    /* HALT     33  0x21 */
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
#define T_OPCODE_ECHO     0x4d  /* print a message to standard output */
#define T_OPCODE_ERR      0x4e  /* print an error */
#define T_OPCODE_PERROR   0x4f  /* print an error (with system error message) */
#define T_OPCODE_BAIL     0x50  /* ... */
#define T_OPCODE_MARK     0x51  /* ... */
#define T_OPCODE_FSTAT    0x52  /* check to see if a file exists */
#define T_OPCODE_ISFILE   0x53  /* is a path a regular file? */
#define T_OPCODE_ISLINK   0x54  /* is a path a symbolic link? */
#define T_OPCODE_ISDIR    0x55  /* is a path a directory? */
#define T_OPCODE_TOUCH    0x56  /* touch a file (create or update its utime) */
#define T_OPCODE_UNLINK   0x57  /* remove a file */
#define T_OPCODE_RENAME   0x58  /* rename a file */
#define T_OPCODE_CHOWN    0x59  /* change file ownership */
#define T_OPCODE_CHGRP    0x5a  /* change file group ownership */
#define T_OPCODE_CHMOD    0x5b  /* change file permissions */
#define T_OPCODE_FSHA1    0x5c  /* calculate SHA1 of a file's contents */
#define T_OPCODE_GETFILE  0x5d  /* retrieve a file from the server */
#define T_OPCODE_GETUID   0x5e  /* look up a user's UID, by name */
#define T_OPCODE_GETGID   0x5f  /* look up a group's GID, by name */
#define T_OPCODE_EXEC     0x60  /* execute a command */
#define T_OPCODE_DUMP     0x61  /* dump virtual machine state for debugging */
#define T_OPCODE_HALT     0x62  /* halt the virtual machine */


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
	"echo",    /* T_OPCODE_ECHO     13  0x0d */
	"err",     /* T_OPCODE_ERR      14  0x0e */
	"perror",  /* T_OPCODE_PERROR   15  0x0f */
	"bail",    /* T_OPCODE_BAIL     16  0x10 */
	"mark",    /* T_OPCODE_MARK     17  0x11 */
	"fstat",   /* T_OPCODE_FSTAT    18  0x12 */
	"isfile",  /* T_OPCODE_ISFILE   19  0x13 */
	"islink",  /* T_OPCODE_ISLINK   20  0x14 */
	"isdir",   /* T_OPCODE_ISDIR    21  0x15 */
	"touch",   /* T_OPCODE_TOUCH    22  0x16 */
	"unlink",  /* T_OPCODE_UNLINK   23  0x17 */
	"rename",  /* T_OPCODE_RENAME   24  0x18 */
	"chown",   /* T_OPCODE_CHOWN    25  0x19 */
	"chgrp",   /* T_OPCODE_CHGRP    26  0x1a */
	"chmod",   /* T_OPCODE_CHMOD    27  0x1b */
	"fsha1",   /* T_OPCODE_FSHA1    28  0x1c */
	"getfile", /* T_OPCODE_GETFILE  29  0x1d */
	"getuid",  /* T_OPCODE_GETUID   30  0x1e */
	"getgid",  /* T_OPCODE_GETGID   31  0x1f */
	"exec",    /* T_OPCODE_EXEC     32  0x20 */
	"dump",    /* T_OPCODE_DUMP     33  0x21 */
	"halt",    /* T_OPCODE_HALT     34  0x22 */
	NULL,
};
#endif

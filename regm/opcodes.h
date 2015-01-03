/** OPCODE CONSTANTS **/
#define NOOP           0000  /* does nothing */
#define PUSH           0x01  /* push a register onto data stack */
#define POP            0x02  /* pop data stack top into a register */
#define SET            0x03  /* set register value */
#define SWAP           0x04  /* swap two register values */
#define CALL           0x05  /* call a user-defined function */
#define RET            0x06  /* return to caller */
#define CMP            0x07  /* compare two integers */
#define STRCMP         0x08  /* compare two strings */
#define JMP            0x09  /* unconditional jump */
#define JZ             0x0a  /* jump if accumulator is 0 */
#define JNZ            0x0b  /* jump if accumulator is not 0 */
#define ECHO           0x0c  /* print a message to standard output */
#define ERR            0x0d  /* print an error */
#define PERROR         0x0e  /* print an error (with system error message) */
#define BAIL           0x0f  /* ... */
#define MARK           0x10  /* ... */
#define FS_STAT        0x11  /* check to see if a file exists */
#define FS_FILE_P      0x12  /* is a path a regular file? */
#define FS_SYMLINK_P   0x13  /* is a path a symbolic link? */
#define FS_DIR_P       0x14  /* is a path a directory? */
#define FS_CHARDEV_P   0x15  /* is a path a character device? */
#define FS_BLOCKDEV_P  0x16  /* is a path a block device? */
#define FS_FIFO_P      0x17  /* is a path a FIFO queue? */
#define FS_SOCKET_P    0x18  /* is a path a socket? */
#define FS_READLINK    0x19  /* get the value of a symbolic link */
#define FS_TOUCH       0x1a  /* touch a file (create or update its utime) */
#define FS_MKDIR       0x1b  /* create a new (empty) directory */
#define FS_LINK        0x1c  /* create a file link */
#define FS_SYMLINK     0x1d  /* create a symbolic link */
#define FS_UNLINK      0x1e  /* remove a file */
#define FS_RMDIR       0x1f  /* remove an empty directory */
#define FS_RENAME      0x20  /* rename a file */
#define FS_COPY        0x21  /* copy a file from one name to another */
#define FS_CHOWN       0x22  /* change file ownership */
#define FS_CHGRP       0x23  /* change file group ownership */
#define FS_CHMOD       0x24  /* change file permissions */
#define FS_SHA1        0x25  /* calculate SHA1 of a file's contents */
#define FS_GET         0x26  /* retrieve the contents of a local file */
#define FS_PUT         0x27  /* update the contents of a local file */
#define GETFILE        0x28  /* retrieve a file from the server */
#define GETUID         0x29  /* look up a user's UID, by name */
#define GETGID         0x2a  /* look up a group's GID, by name */
#define EXEC           0x2b  /* execute a command */
#define DUMP           0x2c  /* dump virtual machine state for debugging */
#define HALT           0x2d  /* halt the virtual machine */


#ifdef OPCODES_EXTENDED
/** OPCODE MNEMONIC NAMES **/
static const char * OPCODES[] = {
	"noop",          /* NOOP            0  0000 */
	"push",          /* PUSH            1  0x01 */
	"pop",           /* POP             2  0x02 */
	"set",           /* SET             3  0x03 */
	"swap",          /* SWAP            4  0x04 */
	"call",          /* CALL            5  0x05 */
	"ret",           /* RET             6  0x06 */
	"cmp",           /* CMP             7  0x07 */
	"strcmp",        /* STRCMP          8  0x08 */
	"jmp",           /* JMP             9  0x09 */
	"jz",            /* JZ             10  0x0a */
	"jnz",           /* JNZ            11  0x0b */
	"echo",          /* ECHO           12  0x0c */
	"err",           /* ERR            13  0x0d */
	"perror",        /* PERROR         14  0x0e */
	"bail",          /* BAIL           15  0x0f */
	"mark",          /* MARK           16  0x10 */
	"fs.stat",       /* FS_STAT        17  0x11 */
	"fs.file?",      /* FS_FILE_P      18  0x12 */
	"fs.symlink?",   /* FS_SYMLINK_P   19  0x13 */
	"fs.dir?",       /* FS_DIR_P       20  0x14 */
	"fs.chardev?",   /* FS_CHARDEV_P   21  0x15 */
	"fs.blockdev?",  /* FS_BLOCKDEV_P  22  0x16 */
	"fs.fifo?",      /* FS_FIFO_P      23  0x17 */
	"fs.socket?",    /* FS_SOCKET_P    24  0x18 */
	"fs.readlink",   /* FS_READLINK    25  0x19 */
	"fs.touch",      /* FS_TOUCH       26  0x1a */
	"fs.mkdir",      /* FS_MKDIR       27  0x1b */
	"fs.link",       /* FS_LINK        28  0x1c */
	"fs.symlink",    /* FS_SYMLINK     29  0x1d */
	"fs.unlink",     /* FS_UNLINK      30  0x1e */
	"fs.rmdir",      /* FS_RMDIR       31  0x1f */
	"fs.rename",     /* FS_RENAME      32  0x20 */
	"fs.copy",       /* FS_COPY        33  0x21 */
	"fs.chown",      /* FS_CHOWN       34  0x22 */
	"fs.chgrp",      /* FS_CHGRP       35  0x23 */
	"fs.chmod",      /* FS_CHMOD       36  0x24 */
	"fs.sha1",       /* FS_SHA1        37  0x25 */
	"fs.get",        /* FS_GET         38  0x26 */
	"fs.put",        /* FS_PUT         39  0x27 */
	"getfile",       /* GETFILE        40  0x28 */
	"getuid",        /* GETUID         41  0x29 */
	"getgid",        /* GETGID         42  0x2a */
	"exec",          /* EXEC           43  0x2b */
	"dump",          /* DUMP           44  0x2c */
	"halt",          /* HALT           45  0x2d */
	NULL,
};


/** ASM TOKENS **/
#define T_OPCODE_NOOP           0x40  /* does nothing */
#define T_OPCODE_PUSH           0x41  /* push a register onto data stack */
#define T_OPCODE_POP            0x42  /* pop data stack top into a register */
#define T_OPCODE_SET            0x43  /* set register value */
#define T_OPCODE_SWAP           0x44  /* swap two register values */
#define T_OPCODE_CALL           0x45  /* call a user-defined function */
#define T_OPCODE_RET            0x46  /* return to caller */
#define T_OPCODE_RETV           0x47  /* return to caller (with value) */
#define T_OPCODE_CMP            0x48  /* compare two integers */
#define T_OPCODE_STRCMP         0x49  /* compare two strings */
#define T_OPCODE_JMP            0x4a  /* unconditional jump */
#define T_OPCODE_JZ             0x4b  /* jump if accumulator is 0 */
#define T_OPCODE_JNZ            0x4c  /* jump if accumulator is not 0 */
#define T_OPCODE_ECHO           0x4d  /* print a message to standard output */
#define T_OPCODE_ERR            0x4e  /* print an error */
#define T_OPCODE_PERROR         0x4f  /* print an error (with system error message) */
#define T_OPCODE_BAIL           0x50  /* ... */
#define T_OPCODE_MARK           0x51  /* ... */
#define T_OPCODE_FS_STAT        0x52  /* check to see if a file exists */
#define T_OPCODE_FS_FILE_P      0x53  /* is a path a regular file? */
#define T_OPCODE_FS_SYMLINK_P   0x54  /* is a path a symbolic link? */
#define T_OPCODE_FS_DIR_P       0x55  /* is a path a directory? */
#define T_OPCODE_FS_CHARDEV_P   0x56  /* is a path a character device? */
#define T_OPCODE_FS_BLOCKDEV_P  0x57  /* is a path a block device? */
#define T_OPCODE_FS_FIFO_P      0x58  /* is a path a FIFO queue? */
#define T_OPCODE_FS_SOCKET_P    0x59  /* is a path a socket? */
#define T_OPCODE_FS_READLINK    0x5a  /* get the value of a symbolic link */
#define T_OPCODE_FS_TOUCH       0x5b  /* touch a file (create or update its utime) */
#define T_OPCODE_FS_MKDIR       0x5c  /* create a new (empty) directory */
#define T_OPCODE_FS_LINK        0x5d  /* create a file link */
#define T_OPCODE_FS_SYMLINK     0x5e  /* create a symbolic link */
#define T_OPCODE_FS_UNLINK      0x5f  /* remove a file */
#define T_OPCODE_FS_RMDIR       0x60  /* remove an empty directory */
#define T_OPCODE_FS_RENAME      0x61  /* rename a file */
#define T_OPCODE_FS_COPY        0x62  /* copy a file from one name to another */
#define T_OPCODE_FS_CHOWN       0x63  /* change file ownership */
#define T_OPCODE_FS_CHGRP       0x64  /* change file group ownership */
#define T_OPCODE_FS_CHMOD       0x65  /* change file permissions */
#define T_OPCODE_FS_SHA1        0x66  /* calculate SHA1 of a file's contents */
#define T_OPCODE_FS_GET         0x67  /* retrieve the contents of a local file */
#define T_OPCODE_FS_PUT         0x68  /* update the contents of a local file */
#define T_OPCODE_GETFILE        0x69  /* retrieve a file from the server */
#define T_OPCODE_GETUID         0x6a  /* look up a user's UID, by name */
#define T_OPCODE_GETGID         0x6b  /* look up a group's GID, by name */
#define T_OPCODE_EXEC           0x6c  /* execute a command */
#define T_OPCODE_DUMP           0x6d  /* dump virtual machine state for debugging */
#define T_OPCODE_HALT           0x6e  /* halt the virtual machine */


static const char * ASM[] = {
	"noop",          /* T_OPCODE_NOOP            0  0000 */
	"push",          /* T_OPCODE_PUSH            1  0x01 */
	"pop",           /* T_OPCODE_POP             2  0x02 */
	"set",           /* T_OPCODE_SET             3  0x03 */
	"swap",          /* T_OPCODE_SWAP            4  0x04 */
	"call",          /* T_OPCODE_CALL            5  0x05 */
	"ret",           /* T_OPCODE_RET             6  0x06 */
	"retv",          /* T_OPCODE_RETV            7  0x07 */
	"cmp",           /* T_OPCODE_CMP             8  0x08 */
	"strcmp",        /* T_OPCODE_STRCMP          9  0x09 */
	"jmp",           /* T_OPCODE_JMP            10  0x0a */
	"jz",            /* T_OPCODE_JZ             11  0x0b */
	"jnz",           /* T_OPCODE_JNZ            12  0x0c */
	"echo",          /* T_OPCODE_ECHO           13  0x0d */
	"err",           /* T_OPCODE_ERR            14  0x0e */
	"perror",        /* T_OPCODE_PERROR         15  0x0f */
	"bail",          /* T_OPCODE_BAIL           16  0x10 */
	"mark",          /* T_OPCODE_MARK           17  0x11 */
	"fs.stat",       /* T_OPCODE_FS_STAT        18  0x12 */
	"fs.file?",      /* T_OPCODE_FS_FILE_P      19  0x13 */
	"fs.symlink?",   /* T_OPCODE_FS_SYMLINK_P   20  0x14 */
	"fs.dir?",       /* T_OPCODE_FS_DIR_P       21  0x15 */
	"fs.chardev?",   /* T_OPCODE_FS_CHARDEV_P   22  0x16 */
	"fs.blockdev?",  /* T_OPCODE_FS_BLOCKDEV_P  23  0x17 */
	"fs.fifo?",      /* T_OPCODE_FS_FIFO_P      24  0x18 */
	"fs.socket?",    /* T_OPCODE_FS_SOCKET_P    25  0x19 */
	"fs.readlink",   /* T_OPCODE_FS_READLINK    26  0x1a */
	"fs.touch",      /* T_OPCODE_FS_TOUCH       27  0x1b */
	"fs.mkdir",      /* T_OPCODE_FS_MKDIR       28  0x1c */
	"fs.link",       /* T_OPCODE_FS_LINK        29  0x1d */
	"fs.symlink",    /* T_OPCODE_FS_SYMLINK     30  0x1e */
	"fs.unlink",     /* T_OPCODE_FS_UNLINK      31  0x1f */
	"fs.rmdir",      /* T_OPCODE_FS_RMDIR       32  0x20 */
	"fs.rename",     /* T_OPCODE_FS_RENAME      33  0x21 */
	"fs.copy",       /* T_OPCODE_FS_COPY        34  0x22 */
	"fs.chown",      /* T_OPCODE_FS_CHOWN       35  0x23 */
	"fs.chgrp",      /* T_OPCODE_FS_CHGRP       36  0x24 */
	"fs.chmod",      /* T_OPCODE_FS_CHMOD       37  0x25 */
	"fs.sha1",       /* T_OPCODE_FS_SHA1        38  0x26 */
	"fs.get",        /* T_OPCODE_FS_GET         39  0x27 */
	"fs.put",        /* T_OPCODE_FS_PUT         40  0x28 */
	"getfile",       /* T_OPCODE_GETFILE        41  0x29 */
	"getuid",        /* T_OPCODE_GETUID         42  0x2a */
	"getgid",        /* T_OPCODE_GETGID         43  0x2b */
	"exec",          /* T_OPCODE_EXEC           44  0x2c */
	"dump",          /* T_OPCODE_DUMP           45  0x2d */
	"halt",          /* T_OPCODE_HALT           46  0x2e */
	NULL,
};
#endif

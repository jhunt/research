/* c pre-amble */
#include <assert.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

void * lex_open(const char *);
void lex_close(void *);
int lex_token(void *);

typedef struct {
	const char *file;

	char   *source;
	size_t  len;

	size_t  line;
	size_t  column;

	size_t  dot;
} lexer;

#define LEXER(v) ((lexer *)(v))

void *
lex_open(const char *file)
{
	int fd = -1;
	void *lx = NULL;

	off_t size;
	ssize_t nread;

	assert(file != NULL);

	lx = malloc(sizeof(lexer));
	if (!lx) goto failed;

	fd = open(file, O_RDONLY);
	if (fd < 0) goto failed;

	size = lseek(fd, 0, SEEK_END);
	if (size < 0) goto failed;

	if (lseek(fd, 0, SEEK_SET) < 0)
		goto failed;

	LEXER(lx)->len = size;
	LEXER(lx)->source = malloc(LEXER(lx)->len);
	if (!LEXER(lx)->source) goto failed;

	nread = read(fd, LEXER(lx)->source, LEXER(lx)->len);
	if (nread < 0) goto failed;
	if ((size_t)nread != LEXER(lx)->len) {
		errno = EIO;
		goto failed;
	}

	LEXER(lx)->line = LEXER(lx)->column = 1;
	LEXER(lx)->dot = 0;

	close(fd);
	return LEXER(lx);

failed:
	if (fd >= 0) close(fd);
	lex_close(lx);
	return NULL;
}

void
lex_close(void *lx) {
	if (!lx) return;
	free(LEXER(lx)->source);
	free(LEXER(lx));
}

const char *
lex_lexeme(void *lx)
{
}

int
lex_token(void *lx)
{
}

int
lex(void *lx)
{
	if (LEXER(lx)->dot == LEXER(lx)->len)
		return 0;
#define recognize(t,l,u)
#undef recognize

	LEXER(lx)->dot++;
	return 1;
}

/* c post-amble */

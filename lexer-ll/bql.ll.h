#ifndef REALLY_LONG_IDENTIFIER_FIXME
#define REALLY_LONG_IDENTIFIER_FIXME

void * lex_open(const char *);
void lex_close(void *);
int lex(void *);

const char * lex_lexeme(void *);
int lex_token(void *);

#endif

/* c pre-amble */
#define TOKEN_VAR     1
#define TOKEN_QSTRING 2
----
a|b|c|x|y|z
  recognize(TOKEN_VAR, NULL, NULL);

".*"
  recognize(TOKEN_QSTRING,
    lexeme(1, ll.lexeme.len - 1),
    ll.lexeme.len - 2);
----
/* c post-amble */
#include <stdio.h>
void done() {
  printf("all done!\n");
}

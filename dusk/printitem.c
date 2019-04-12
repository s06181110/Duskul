/* Duskul version 0.1.5,  2018.08.12,   Takeshi Ogihara, (C) 2018 */
#include <stdlib.h>
#include <string.h>
#include "abort.h"
#include "identifiers.h"
#include "strliteral.h"

#define SHORTEN_LEN  80

/// Shorten strings, taking UTF-8 into account.
const char *shorten(const char *s, int limit)
{
    static char buf[SHORTEN_LEN + 8];
    int i, cc = 0;
    int lim = (SHORTEN_LEN < limit) ? SHORTEN_LEN : limit;
    for (i = 0; (cc = s[i]) != 0 && (i < lim || (cc & 0xc0) == 0x80); i++)
        buf[i] = cc; /* Refer to the encoding of UTF-8 */
    if (cc)
        for (int k = 0; k < 3; k++) buf[i++] = '.';
    buf[i] = 0;
    return buf;
}

void itemToString(char *buffer, const item *p)
{
    const char *str;
    token_t t = p->token;
    switch (t) {
        case tok_id:
            sprintf(buffer, "[id \"%s\"]", p->a.recptr->str);
            break;
        case tok_num:
            sprintf(buffer, "[int %ld]", p->a.value); break;
        case tok_str:
            str = shorten(literalString((int)p->a.value), 20);
            sprintf(buffer, "[string \"%s\"]", str); break;
        default:
            if (t < combined_symbol_0)
                sprintf(buffer, "[symbol '%c']", (int)p->kind);
            else if (t < reserved_word_0) {
                static const char *tab[] = { "<=", ">=", "==", "<>" };
                sprintf(buffer, "[symbol '%s']", tab[t - combined_symbol_0]);
            }else if (t < all_normal_symbols) {
                sprintf(buffer, "[token '%s']", reservedWord(t));
            }else
                sprintf(buffer, "[token ILL/EOF]");
            break;
    }
}

#if defined(DEBUG)
void printItem(const item *p)
{
    char buffer[80];
    itemToString(buffer, p);
    fprintf(stderr, "%s\n", buffer);
}
#endif

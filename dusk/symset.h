/* Duskul version 0.1.2,  2018.05.13,   Takeshi Ogihara, (C) 2018 */
#if !defined(__SYMSET_H__)
#define __SYMSET_H__

#include "token.h"

#define BITS_IN_WORD     (sizeof(unsigned long) * 8)
#define WORDS_OF_SYMSET  ((all_normal_symbols + BITS_IN_WORD - 1) / BITS_IN_WORD)

/** The type symset_t is a set of token_t.  It is used to express a set
   of tokens that may appear during parsing.  The set operations of
   Union, Add, and Test are defined here.
 */
typedef struct {
    unsigned long bits[WORDS_OF_SYMSET];
} symset_t;

/* symset.c */
extern symset_t prefix_set, infix_set;

void symInitialize(void);
symset_t symsetCreate(token_t initset[]);
// The last data of initset[] should be "tok_EOD".
void symsetUnion(symset_t *a, symset_t b);
void symsetAdd(symset_t *a, int elm);
bool symsetHas(const symset_t sset, int elm);

#endif /* !__SYMSET_H__ */

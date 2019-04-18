/* Duskul version 0.1.1,  2018.03.13,   Takeshi Ogihara, (C) 2018 */
#include "symset.h"
#include "abort.h"

/* extern */
symset_t prefix_set, infix_set;

void symInitialize(void) {
    // C99 style: literal of an array
    prefix_set = symsetCreate((token_t[]){
        sym_plus, sym_minus, sym_not, tok_EOD }
    );
    infix_set = symsetCreate((token_t[]){
        sym_and, sym_or, sym_equal, sym_neq, sym_gt, sym_lt, sym_geq,
        sym_leq, sym_plus, sym_minus, sym_ast, sym_sls, sym_pcnt, tok_EOD }
    );
}

symset_t symsetCreate(token_t initset[])
{
    int i, elm;
    static symset_t symset_init = { { 0 } };
    symset_t sset = symset_init;
    for (i = 0; (elm = (int)initset[i]) < tok_EOD; i++) {
        int x = elm / BITS_IN_WORD;
        int b = elm % BITS_IN_WORD;
        sset.bits[x] |= 01UL << b;
    }
    return sset;
}

void symsetUnion(symset_t *a, symset_t b)
{
    for (int i = 0; i < WORDS_OF_SYMSET; i++)
        a->bits[i] |= b.bits[i];
}

void symsetAdd(symset_t *sset, int elm)
{
    assert(elm < all_normal_symbols);
    int x = elm / BITS_IN_WORD;
    int b = elm % BITS_IN_WORD;
    sset->bits[x] |= 01UL << b;
}

bool symsetHas(const symset_t sset, int elm)
{
    assert(elm < all_normal_symbols);
    int x = elm / BITS_IN_WORD;
    int b = elm % BITS_IN_WORD;
    return BOOL(sset.bits[x] & (01UL << b));
}

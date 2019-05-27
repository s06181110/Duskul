/* Duskul version 0.1.1,  2018.03.13,   Takeshi Ogihara, (C) 2018 */
#if !defined(__EXP_IMP_H__)
#define __EXP_IMP_H__

#include "getitem.h"
#include "stnode_imp.h"

#define ExpTree     tok_ILL // mark of 'oprExpnode'

// Note: expnode, oprExpnode, and argExpnode have the common part:
// kind and prefix.

struct _expnode {
    token_t  kind;      // tok_id:variable, tok_num:int, tok_str: string-literal
    token_t  prefix;    // 0, sym_plus, sym_minus, sym_not
    union {             //型longとしても構造体varinfoとしても使える。
        long    intvalue;   // if kind == tok_num, or tok_str  long型の整数として
        varinfo varinf;     // if kind == tok_id  varinfoの構造体として
    } v;
};

struct _oprExpnode {
    token_t  kind;          // operators: sym_ast, sym_plus, ...
    token_t  prefix;        // always ExpTree
    expnode  *operand[2];
};

struct _argExpnode {
    token_t kind;       // sym_func
    token_t prefix;     // 0, sym_plus, sym_minus, sym_not
    short   index;      // index of functionsTable
    short   count;      // number of arguments
    expnode *args[1];
};

#endif /* !__EXP_IMP_H__ */

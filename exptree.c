/* Duskul version 0.1.1,  2018.03.13,   Takeshi Ogihara, (C) 2018 */
#include <stdlib.h>
#include "token.h"
#include "expression.h"
#include "exp_imp.h"
#include "stnode_imp.h"
#include "statements.h"
#include "abort.h"


expnode *newExpnode(int prefix, int kind)
{
    assert(kind == tok_id || kind == tok_num || kind == tok_str);
    expnode *xp = malloc(sizeof(expnode));
    xp->kind = kind;
    xp->prefix = prefix;
    return xp;
}

expnode *newOprnode(int kind, expnode *o1, expnode *o2)
{
    oprExpnode *xp = malloc(sizeof(oprExpnode));
    xp->kind = kind;
    xp->prefix = ExpTree;   // always ExpTree
    xp->operand[0] = o1;
    xp->operand[1] = o2;    // kindが単項演算子として使われる場合、ここは NULL
    return (expnode *)xp;
}

argExpnode *newArgnode(int prefix, int funcindex, int argnum)
{
    size_t sz = sizeof(argExpnode);
    if (argnum > 1)
        sz += sizeof(expnode *) * (argnum - 1);
    argExpnode *agp = malloc(sz);
    agp->kind = sym_func;
    agp->prefix = prefix;
    agp->index = funcindex;
    agp->count = argnum;
    return agp;
}

expnode *term(void)
{
    item s = getItem();
    expnode *termp = NULL;
    int prefix = 0; // none
    if (symsetHas(prefix_set, s.token)) {
        prefix = s.token;
        s = getItem();
    }
    if (s.token == sym_lpar) { // ( exp ... )
        termp = expression();
        s = getItem();
        if (s.token != sym_rpar || termp == NULL)
            abortMessageWithToken("no right paren", &s);
        if (prefix == 0 || prefix == sym_plus) {
            // 単項演算子がないか、'+' の時は ( ) の内部をそのまま式にする。
            // e.g. ( 100 ) ,  +( -a * 10 )
            return termp;
        }
        if (termp->prefix != ExpTree && prefix == termp->prefix) {
            // ( ) 内が二項演算子の式ではなく、かつ同じ単項演算子が繰り返されている場合。
            // e.g. -( -f1(0) ) ,  not( not a )
            termp->prefix = 0;
            return termp;
        }
        // ( ) 内が二項演算子の式か、または ( ) の中と外の単項演算子が違う場合。
        // termp->prefix == ExpTree || prefix != termp->prefix
        // e.g. -( c + 10 ) ,  not( a or z ),  -( not f )
        return newOprnode(prefix, termp, NULL);
    }

    if (s.token == tok_id) { // var or func
        if (s.kind == id_func) {
            int num = functionsTable[s.offset]->params;
            argExpnode *agp = newArgnode(prefix, s.offset, num);
            expressionList(agp->args, num);
            return (expnode *)agp;
        }else if (s.kind == id_proc) {
            abortMessageWithToken("not func name", &s);
        }else if (s.kind == id_undefined)
            abortMessageWithToken("undef id", &s);
        // variable
        varinfo info;
        info.global = BOOL(s.kind == id_static_v);
        info.offset = s.offset;
        termp = newExpnode(prefix, tok_id);
        termp->v.varinf = info;
        return termp;
    }

    if (s.token == tok_num) {
        termp = newExpnode(prefix, tok_num);
        termp->v.intvalue = s.a.value;
        return termp;
    }

    abortMessageWithToken("wrong exp", &s);
    return termp;
}

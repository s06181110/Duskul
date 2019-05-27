/* Duskul version 0.1.3,  2018.05.29,   Takeshi Ogihara, (C) 2018 */
#include <stdlib.h>
#include "statements.h"
#include "expression.h"
#include "exp_imp.h"
#include "abort.h"

static int precedence(token_t op)     ///2項演算子を表すトークンを引数として、演算子の優先順位を返す。数が大きいほど優先順位が高い。
{
    switch (op) {
        case sym_or: return 0;
        case sym_and: return 1;
        case sym_plus:
        case sym_minus: return 3;
        case sym_ast:
        case sym_sls:
        case sym_pcnt: return 4;
        default: break;
    }
    return 2;   // sym_equal, sym_neq, sym_gt, sym_lt, sym_geq, sym_leq
}

//
// Operator Precedence Parsing Algorithm
//
#define STACK_MAX   8       // STACK_MAX > (steps of precedence + 1)

typedef struct {
    token_t         opstack[STACK_MAX];     // operators 演算子
    unsigned char   prec[STACK_MAX];        // the precedence of each operator 演算子の優先順位
    expnode         *nodestack[STACK_MAX];  // 項を表す木へのポインタを格納
    int nodindex;                           // 上の配列の末尾 + 1　初期値は0
} oppbody;

static void oppPutOperator(oppbody *opp, int operator)
{
    int prec = (operator == 0) ? (-1) : precedence(operator);
    // (-1) shows the end of the sequence of terms.
    int top = opp->nodindex - 1;
    while (top > 0 && opp->prec[top - 1] >= prec) {    //構造体の中に、引数の演算子の優先順位よりも高いものが存在する限り
        top = --(opp->nodindex) - 1;
        int top_opr = opp->opstack[top];
        expnode **oprn = &opp->nodestack[top];
        oprn[0] = newOprnode(top_opr, oprn[0], oprn[1]);
    }
    opp->opstack[top] = operator;   //演算子と演算子を構造体に格納
    opp->prec[top] = prec;
}

expnode *expression(void)     //項に相当する構文木を返す関数term()を次のトークンが二項演算子である限り繰り返して呼び出す。
{
    oppbody opp;
    item s;
    opp.nodindex = 0;
    for ( ; ; ) {
        opp.nodestack[opp.nodindex++] = term();
        s = getItem();
        if (!symsetHas(infix_set, s.token))    //項の後に演算子が出現しなくなったら式の評価を終える。
            break;
        oppPutOperator(&opp, s.token);
    }
    ungetItem(s);
    oppPutOperator(&opp, 0); // to finish the processing
    return opp.nodestack[0];    //式全体を表す木構造へのポインタを関数の値として返す。
}

expnode *strExpression(void)
{
    item s = getItem();
    if (s.token == tok_str) {
        expnode *exp = newExpnode(0, tok_str);
        exp->v.intvalue = s.a.value;
        return exp;
    }
    ungetItem(s);
    return expression();
}

void expressionList(expnode *xlist[], int args)
{
    item s = getItem();
    if (s.token != sym_lpar)
        abortMessageWithToken("no left paren", &s);
    if (args > 0) {
        for (int i = 0; ; ) {
            xlist[i] = expression();
            if (++i >= args) break;
            s = getItem();
            if (s.token != sym_comma)
                abortMessageWithToken("wrong arg num", &s);
        }
    }
    s = getItem();
    if (s.token != sym_rpar)
        abortMessageWithToken("no right paren", &s);
}


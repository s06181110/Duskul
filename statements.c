/* Duskul version 0.1.1,  2018.03.13,   Takeshi Ogihara, (C) 2018 */
#include <stdio.h>
#include <stdlib.h>
#include "getitem.h"
#include "identifiers.h"
#include "statements.h"
#include "stnode_imp.h"
#include "expression.h"
#include "abort.h"

/* extern */
int currentBreakNest = 0;
bool valueIsReturned = false;
symset_t end_set;

static symset_t afterthen_set, stat_set, rtn_set, utl_set;

void statInitialize(void)
{
    // C99 style literal
    stat_set = symsetCreate((token_t[]){
        tok_id, sym_call, sym_if, sym_while, sym_for, sym_repeat,
        sym_print, sym_println, sym_input, tok_EOD });
    end_set = symsetCreate((token_t[]){ sym_end, tok_EOD });
    rtn_set = symsetCreate((token_t[]){ sym_break, sym_return, tok_EOD });
    utl_set = symsetCreate((token_t[]){ sym_until, tok_EOD });
    afterthen_set = symsetCreate((token_t[]){ sym_else, sym_elsif, tok_EOD });
    symsetUnion(&afterthen_set, end_set);
}

#define MAX_IF_SEQ      64

/// if文："if"を読んでから呼び出される
static stnode *ifStatement(void)
{
    struct ifclause buffer[MAX_IF_SEQ];
    item s;
    int index = 0;
    do {
        buffer[index].expr = expression();
        s = getItem();
        if (s.token != sym_then)
            abortMessageWithToken("no then", &s);
        blockNestPush();
        buffer[index].body = codeblock(afterthen_set, false);
        blockNestPop();
        if (++index >= MAX_IF_SEQ - 1)
            abortMessageWithString("many if", "elsif");
        s = getItem();
    }while (s.token == sym_elsif);
    if (s.token == sym_else) {
        buffer[index].expr = NULL;
        blockNestPush();
        buffer[index++].body = codeblock(end_set, false);
        blockNestPop();
        (void)getItem();
    }
    stnode *stp = newNodeExpand(node_if, index);
    ifnode *ifp = (ifnode *)stp;
    for (int x = 0; x < index; x++)
        ifp->clause[x] = buffer[x];
    return stp;
}

static stnode *whileStatement(void)
{
    stnode *stp = newNode(node_while);
    whilenode *whp = (whilenode *)stp;
    whp->expr = expression();
    item s = getItem();
    if (s.token != sym_do)
        abortMessageWithToken("no do", &s);
    currentBreakNest++;
    blockNestPush();
    whp->body = codeblock(end_set, false);
    blockNestPop();
    currentBreakNest--;
    (void)getItem();
    return stp;
}

static stnode *repeatStatement(void)
{
    stnode *stp =newNode(node_repeat);
    repeatnode *rep =(repeatnode *)stp;
    currentBreakNest++;
    blockNestPush();
    rep->body =codeblock(utl_set, false);
    blockNestPop();
    currentBreakNest--;
    return stp;
}
     
static stnode *callStatement(void)
{
    item pr = getItem();
    if (pr.token != tok_id || pr.kind != id_proc)
        abortMessageWithToken("not proc name", &pr);
    int num = functionsTable[pr.offset]->params;
    stnode *stp = newNodeExpand(node_call, num);
    stp->count = num;
    argnode *anp = (argnode *)stp;
    anp->offset = pr.offset;
    expressionList(anp->p.xlist, num);
    return stp;
}

static stnode *printStatement(int sym)
{
    expnode *buffer[PARAM_MAX];
    item s = getItem();
    if (s.token != sym_lpar)
        abortMessageWithToken("no left paren", &s);
    int num = 0;
    s = getItem();
    if (s.token != sym_rpar) {
        ungetItem(s);
        do {
            if (num >= PARAM_MAX)
                abortMessageWithString("wrong arg num", "print");
            buffer[num++] = strExpression();
            s = getItem();
        }while(s.token == sym_comma);
        if (s.token != sym_rpar)
            abortMessageWithToken("no right paren", &s);
    }
    stnode *stp = newNodeExpand(node_print, num);
    stp->count = num;
    argnode *anp = (argnode *)stp;
    anp->offset = BOOL(sym == sym_println); // need new-line
    if (num > 0) {
        expnode **xlist = anp->p.xlist;
        for (int i = 0; i < num; i++)
            xlist[i] = buffer[i];
    }
    return stp;
}

stnode *fetchStatement(item ahead)
{
    switch (ahead.token) {
        case sym_call:
            return callStatement();
        case sym_print:
        case sym_println:
            return printStatement(ahead.token);
        case sym_input:
            return inputStatement();
        case sym_if:
            return ifStatement();
        case sym_while:
            return whileStatement();
        case sym_for:
            return forStatement();
        case sym_repeat:
            return repeatStatement();
        default:
            break; // error
    }
    assert(false); // error
    return NULL;
}

/// 変数宣言: 'var' が読まれてから呼び出される。',' が出現しなくなったら終わり。
int varsDeclareList(int offset, bool toplevel)
{
    item s;
    int vars = offset;
    do {
        s = getItemLocal();
        if (s.token != tok_id) abortMessage("no id");
        if (s.kind != id_undefined) abortMessage("id conflict");
        idRecord *ent = s.a.recptr;
        ent->kind = toplevel ? id_static_v : id_local_v;
        ent->order = vars++;
        s = getItem();
    }while (s.token == sym_comma);
    ungetItem(s);
    return vars;
}

/// 文列：終わりを表すトークンの集合を第１引数に与える。終わりのトークンは ungetItem() される。
stnode *codeblock(symset_t termset, bool rtnflag)
{
    stnode *root = NULL;
    stnode **statmp = &root;
    stnode *nodp = NULL;
    symset_t exit_set = termset;        // sym_end or sym_elsif, etc.
    symsetUnion(&exit_set, rtn_set);    // += sym_return, sym_break
    symset_t assign_set = exit_set;
    symsetUnion(&assign_set, stat_set); // += while, for, if, call, etc.
    int vars = currentLocalOffset;
    item s = getItem();
    while (s.token == sym_var) {
        vars = varsDeclareList(vars, false);
        s = getItem();
    }
    currentLocalOffset = vars;
    while (symsetHas(stat_set, s.token)) {
        nodp = (s.token == tok_id)
             ? assignStatement(s, assign_set) : fetchStatement(s);
        *statmp = nodp;
        statmp = &nodp->next;
        s = getItem();
    }
    if (!symsetHas(exit_set, s.token))  // terminator of statement-list ?
        abortMessageWithToken("illegal statement", &s);
    bool has_rtn = false;
    if (s.token == sym_return || s.token == sym_break) {
        if (s.token == sym_break) {
            if (currentBreakNest == 0)
                abortMessage("illegal break");
            nodp = newNode(node_break);
        }else { // sym_return
            nodp = returnStatement(termset);
            has_rtn = true;
        }
        *statmp = nodp;
        s = getItem();
    }else if (s.token == sym_until){
		// repeatはuntilを読んだら終了のため、rootを返す（ゴリ押しかも）
        nodp = untilStatement(termset); // ここでuntilの後ろに続く式を解析
        *statmp = nodp;
        return root;
    }
    if (!symsetHas(termset, s.token)) {
        const char *msg = symsetHas(termset, sym_end)
                          ? "no end" : "illegal statement";
        abortMessageWithToken(msg, &s);
    }
    ungetItem(s);
    if (rtnflag && !has_rtn)
        abortMessage("return needed");
    return root;
}

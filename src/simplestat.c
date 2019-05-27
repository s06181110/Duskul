/* Duskul version 0.1.1,  2018.03.13,   Takeshi Ogihara, (C) 2018 */
/* Duskul version 1.0.0,  2019.03.20 */
#include "getitem.h"
#include "identifiers.h"
#include "statements.h"
#include "stnode_imp.h"
#include "expression.h"
#include "abort.h"

static void chechAssignment(idkind_t kind, const char *str)
{
    switch (kind) {
        case id_param:
            abortMessageWithString("assign param", str); break;
        case id_func:
        case id_proc:
            abortMessageWithString("assign func", str); break;
        case id_undefined:
        case id_new:
            abortMessageWithString("undef id", str); break;
        default:
            break;
    }
}

stnode *assignStatement(item ahead, symset_t terminator)
{
    chechAssignment(ahead.kind, "assign");
    item s = getItem();
    if (s.token != sym_eq)
        abortMessageWithToken("no equal", &s);
    expnode *termp = expression();
    stnode *statmp = newNode(node_assign);
    assignnode *ap = (assignnode *)statmp;
    ap->expr = termp;
    ap->global = BOOL(ahead.kind == id_static_v);
    ap->offset = ahead.offset;
    s = getItem();
    if (!symsetHas(terminator, s.token))
        abortMessageWithToken("illegal tail", &s);
    ungetItem(s);
    return statmp;
}

stnode *returnStatement(symset_t terminator)
{
    expnode *termp = valueIsReturned ? expression() : NULL;
    stnode *statmp = newNode(node_return);
    assignnode *rtnp = (assignnode *)statmp;
    rtnp->expr = termp;
    rtnp->global = valueIsReturned; // with expression?
    rtnp->offset = currentFuncIndex;
    item s = getItem();
    if (!symsetHas(terminator, s.token))
        abortMessageWithToken("illegal tail", &s);
    ungetItem(s);
    return statmp;
}

stnode *inputStatement(void)
{
    varinfo buffer[PARAM_MAX];
    item s = getItem();
    if (s.token != sym_lpar)
        abortMessageWithToken("no left paren", &s);
    int args = 0;
    for ( ; ; ) {
        item v = getItem();
        if (v.token != tok_id)
            abortMessageWithToken("no id", &v);
        chechAssignment(v.kind, "intput");
        buffer[args].global = BOOL(v.kind == id_static_v);
        buffer[args].offset = v.offset;
        args++;
        s = getItem();
        if (s.token != sym_comma) break;
        if (args >= PARAM_MAX)
            abortMessageWithString("wrong arg num", "input");
    }
    if (s.token != sym_rpar)
        abortMessageWithToken("no right paren", &s);

    stnode *stp = newNodeExpand(node_input, args);
    stp->count = args;
    argnode *anp = (argnode *)stp;
    anp->offset = 0;
    if (args > 0) {
        varinfo *vlist = anp->p.vlist;
        for (int i = 0; i < args; i++)
            vlist[i] = buffer[i];
    }
    return stp;
}

stnode *forStatement(void)
{
    stnode *stp = newNode(node_for);
    fornode *fop = (fornode *)stp;
    blockNestPush();
    item s = getItem();
    if (s.token == sym_var) {
        s = getItemLocal();
        if (s.token != tok_id)
            abortMessageWithToken("no id", &s);
        assert(s.kind == id_undefined);
        idRecord *ent = s.a.recptr;
        ent->kind = id_local_v;
        ent->order = currentLocalOffset++;
        fop->global = false;
        fop->offset = ent->order;
    }else {
        if (s.token != tok_id)
            abortMessageWithToken("no id", &s);
        chechAssignment(s.kind, "for");
        fop->global = BOOL(s.kind == id_static_v);
        fop->offset = s.offset;
    }
    s = getItem();
    if (s.token != sym_eq)
        abortMessageWithToken("no equal", &s);
    fop->exps[0] = expression();
    s = getItem();
    if (s.token != sym_to)
        abortMessageWithToken("no to", &s);
    fop->exps[1] = expression();
    s = getItem();
    if (s.token == sym_step) {
        fop->exps[2] = expression();
        stp->count = 3;
        s = getItem();
    }else {
        fop->exps[2] = NULL;
        stp->count = 2;
    }
    if (s.token != sym_do)
        abortMessageWithToken("no do", &s);
    currentBreakNest++;
    fop->body = codeblock(end_set, false);
    currentBreakNest--;
    blockNestPop();
    (void)getItem();
    return stp;
}

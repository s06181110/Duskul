/* Duskul version 0.1.5,  2018.08.12,   Takeshi Ogihara, (C) 2018 */
/* Duskul version 1.0.0,  2019.03.20 */
#include <stdlib.h>
#include <string.h>
#include "getitem.h"
#include "identifiers.h"
#include "statements.h"
#include "stnode_imp.h"
#include "abort.h"

#define FUNC_TABLE_CAPACITY   32

/* extern */
funcinfo *functionsTable[FUNC_TABLE_CAPACITY];
    // 関数、手続きの情報を格納する配列。構文木の根でもある。
int currentFuncIndex = 0;
int numberOfFunctions = 0;
int numberOfStaticVars = 0;

// 仮引数列: '(' が読まれてから呼び出される。最後の ')' は読む。
static int parameter_list(void)
{
    item s = getItemLocal();
    if (s.token == sym_rpar) // no parameters
        return 0;

    int prm = 0;
    for ( ; ; ) {
        if (s.token != tok_id) abortMessage("no id");
        if (s.kind != id_undefined) abortMessage("w-def param");
        idRecord *ent = s.a.recptr;
        ent->kind = id_param;
        ent->order = prm++;
        if (prm >= PARAM_MAX) abortMessage("many param");
        s = getItemLocal();
        if (s.token != sym_comma)
            break;
        s = getItemLocal();
    }
    if (s.token != sym_rpar) abortMessage("no right paren");
    return prm;
}

// 'func' or 'proc' が読まれてから呼び出される
// funcinfoをfunctionsTable[]に登録してその添字を返す
// 前方参照があった場合、すでに登録された分の添字を返す
static int func_header(bool isfunc, bool withbody)
{
    int funcindex = numberOfFunctions;
    bool hasproto = false;
    int func_proc = isfunc ? id_func : id_proc;
    item s = getItemLocal();
    if (s.token != tok_id)
        abortMessageWithToken("no id", &s);
    funcinfo *finf = NULL;
    if (s.kind == id_undefined) { // new function
        idRecord *idp = s.a.recptr;
        idp->kind = func_proc;
        idp->order = funcindex = numberOfFunctions++;
        if (numberOfFunctions >= FUNC_TABLE_CAPACITY)
            abortMessage("many func");
        functionsTable[funcindex] = finf = malloc(sizeof(funcinfo));
        finf->ident = idp->str;
        finf->withbody = withbody;
        finf->rtntype = isfunc;
    }else if (s.kind == id_func || s.kind == id_proc) {
        hasproto = true;
        if (s.kind != func_proc)
            abortMessageWithToken("wrong proto", &s);
        funcindex = s.offset;
        finf = functionsTable[funcindex];
        if (finf->withbody && withbody)
            abortMessageWithToken("w-def func", &s);
        if (withbody)
            finf->withbody = true;
    }else {
        abortMessageWithToken("not func name", &s);
    }

    item t = getItem();
    if (t.token != sym_lpar)
        abortMessageWithToken("no left paren", &t);
    resetLocalList();
    blockNestPush(); // pop is called in funcDefine() or funcDeclare()
    int prms = parameter_list();
    if (hasproto) {
        if (finf->params != prms) abortMessageWithToken("wrong proto", &s);
    }else finf->params = prms;
    return funcindex;
}

/// Definition of a subroutine.
/// This is called after a token of 'func' or 'proc' is read.
static void funcDefine(bool isfunc)
{
    int fidx = func_header(isfunc, true); // blockNestPush() is called
    funcinfo *fip = functionsTable[fidx];
    currentFuncIndex = fidx;
    currentBreakNest = 0;
    int first_local_v = currentLocalOffset = fip->params + CONTROL_INFO_SIZE;
    valueIsReturned = isfunc;
    fip->body = codeblock(end_set, valueIsReturned);
    blockNestPop(); // maxLocalOffset is updated
    fip->rtntype = isfunc;
    fip->localvars = maxLocalOffset - first_local_v;  // number of local variables
    (void)getItem();
}

/// Forward declaration: This is called after a token of 'declare' is read.
static void funcDeclare(void)
{
    item s = getItem();
    bool isfunc = false;
    if (s.token == sym_func) isfunc = true;
    else if (s.token != sym_proc)
        abortMessage("no func");
    int fidx = func_header(isfunc, false); // blockNestPush() is called
    funcinfo *fip = functionsTable[fidx];
    fip->rtntype = isfunc;
    blockNestPop();
}

/// Parse the top level definitions of subroutines and vars.
int parseProgram(void)
{
    int vars = 0;
    while (!getEOF()) {
        item s = getItem();
        if (s.token == sym_func || s.token == sym_proc) {
            bool isfunc = BOOL(s.token == sym_func);
            funcDefine(isfunc);
        }else if (s.token == sym_var)
            vars = varsDeclareList(vars, true);
        else if (s.token == sym_decl)
            funcDeclare();
        else {
            abortMessage("func or var");
        }
    }
    numberOfStaticVars = vars;

    int mainindex = -1;
    for (int i = 0; i < numberOfFunctions; i++) {
        const char *name = functionsTable[i]->ident;
        if (!functionsTable[i]->withbody)
            abortMessageWithString("no func body", name);
        if (strcmp(name, "main") == 0)
            mainindex = i;
    }
    return mainindex;
}

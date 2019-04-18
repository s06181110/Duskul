/* Duskul version 0.1.1,  2018.03.13,   Takeshi Ogihara, (C) 2018 */
/* Duskul version 1.0.2,  2019.03.27 */
#if !defined(__STATEMENTS_H__)
#define __STATEMENTS_H__
#include "commontype.h"
#include "stnode.h"
#include "token.h"
#include "getitem.h"
#include "symset.h"

#define PARAM_MAX           16
#define CONTROL_INFO_SIZE   2   // 実引数と局所変数の間に置かれる制御用ワードの数

/* newnode.c */
stnode *newNode(nodetype kind);
stnode *newNodeExpand(nodetype kind, int count);

/* functions.c */
extern funcinfo *functionsTable[];
extern int currentFuncIndex;
extern int numberOfFunctions;
extern int numberOfStaticVars;

int parseProgram(void);

/* statements.c */
void statInitialize(void);
extern int currentBreakNest;
extern bool valueIsReturned;
extern symset_t end_set;

int varsDeclareList(int offset, bool toplevel);
stnode *fetchStatement(item ahead);
stnode *codeblock(symset_t termset, bool rtnflag);

/* simplestat.c */
stnode *assignStatement(item ahead, symset_t terminator);
stnode *returnStatement(symset_t terminator);
stnode *inputStatement(void);
stnode *forStatement(void);

#endif /* !__STATEMENTS_H__ */

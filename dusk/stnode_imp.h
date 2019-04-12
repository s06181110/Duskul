/* Duskul version 1.0.2,  2018.03.25,   Takeshi Ogihara, (C) 2018 */
#ifndef _STNODE_IMP_H_
#define _STNODE_IMP_H_
#include "stnode.h"
#include "expnode.h"

struct _funcinfo {
    const char     *ident;
    stnode         *body;
    unsigned char  withbody;    // not prototype
    unsigned char  rtntype;     // 0:proc, 1:func
    short          params;      // number of parameters
    short          localvars;   // number of local variables
};

struct _assign {                // assign and return
    struct _stnode _padding;    // caution!
    unsigned char  global;
    unsigned short offset;
    // assign: global=[var is global?], offset=[offset of var]
    // return: global=[with expression?], offset=[--not used--]
    expnode        *expr;
};

struct ifclause {
    expnode        *expr;
    struct _stnode *body;
};

struct _ifnode {                // if-statement
    struct _stnode _padding;    // caution!
    struct ifclause  clause[1];
};

struct _whilenode {             // while-statement
    struct _stnode _padding;    // caution!
    expnode        *expr;
    struct _stnode *body;
};

struct _fornode {               // for-statement
    struct _stnode _padding;    // caution!
    unsigned char  global;      // var is global ?
    unsigned short offset;      // offset of var
    expnode        *exps[3];
    struct _stnode *body;       // code block
};

struct _varinfo {
    unsigned char  global; // var is global ?
    unsigned short offset; // offset of var list
};

struct _argnode {
    struct _stnode _padding;        // caution!
    unsigned short offset;          // index of proc / println ?
    union {
        expnode        *xlist[1];   // call, print, println
        varinfo        vlist[1];    // input
    } p; // parameters
};

#endif /* _STNODE_IMP_H_ */

/* Duskul version 1.0.2,  2018.03.25,   Takeshi Ogihara, (C) 2018 */
#ifndef expnode_h
#define expnode_h

struct _expnode;
typedef struct _expnode  expnode;
struct _oprExpnode;
typedef struct _oprExpnode  oprExpnode;
struct _argExpnode;
typedef struct _argExpnode  argExpnode;

expnode *newExpnode(int prefix, int kind);
expnode *newOprnode(int kind, expnode *o1, expnode *o2);
argExpnode *newArgnode(int prefix, int funcindex, int argnum);
expnode *term(void);

#endif /* expnode_h */

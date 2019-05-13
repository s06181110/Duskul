/* Duskul version 0.1.2,  2018.05.13,   Takeshi Ogihara, (C) 2018 */
#include <stdlib.h>
#include <stddef.h>
#include "commontype.h"
#include "stnode_imp.h"
#include "abort.h"

/** Allocate a node for assign, return, while, for, or break. */
stnode *newNode(nodetype kind)
{
    size_t total = sizeof(stnode);
    switch (kind) {
        case node_assign:
        case node_return:
            total = sizeof(assignnode); break;
        case node_while:
            total = sizeof(whilenode); break;
        case node_repeat:
            total = sizeof(repeatnode); break;
        case node_until:
            total = sizeof(untilnode); break;
        case node_for:
            total = sizeof(fornode); break;
        case node_break:
            break;
        default: assert(false); break;
    }
    stnode *p = malloc(total);
    p->next = NULL;
    p->kind = kind;
    p->count = 0;
    return p;
}

/** Allocate a node for if, input, call, print, or println. */
stnode *newNodeExpand(nodetype kind, int count)
{
    size_t total = sizeof(argnode);
    int cnt = (count <= 0) ? 1 : count;

    switch (kind) {
        case node_if:
            total = offsetof(ifnode, clause)  // offsetof() is C99-style
            + sizeof(struct ifclause) * cnt;
            break;
        case node_input:
            total = offsetof(argnode, p) + sizeof(varinfo) * cnt;
            break;
        case node_call:
        case node_print:
            total = offsetof(argnode, p) + sizeof(expnode *) * cnt;
            break;
        default: assert(false); break;
    }
    stnode *p = malloc(total);
    p->next = NULL;
    p->kind = kind;
    p->count = count;
    return p;
}

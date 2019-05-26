/* Duskul version 0.1.5,  2018.08.12,   Takeshi Ogihara, (C) 2018 */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "evaluate.h"
#include "expression.h"
#include "exp_imp.h"
#include "statements.h"
#include "stnode_imp.h"
#include "strliteral.h"
#include "abort.h"
#include <stdlib.h>

static void applyOperator(int opr) // ++sp by this call  演算子適用？
{
    long val = 0;
    long o2 = stack[sp++];  //stackは配列と見なしてよい、++=pop操作
    long o1 = stack[sp];
    switch (opr) {
        case sym_or: val = BOOL((o1 != 0) || (o2 != 0)); break;
        case sym_and: val = BOOL((o1 != 0) && (o2 != 0)); break;
        case sym_plus: val = o1 + o2; break;
        case sym_minus: val = o1 - o2; break;
        case sym_ast: val = o1 * o2; break;
        case sym_sls:
            if (o2 == 0) abortMessageWithString("arith exception", "/");
            val = o1 / o2; break;
        case sym_pcnt:
            if (o2 == 0) abortMessageWithString("arith exception", "%");
            val = o1 % o2; break;
        case sym_equal: val = BOOL(o1 == o2); break;
        case sym_neq:   val = BOOL(o1 != o2); break;
        case sym_gt:    val = BOOL(o1 > o2); break;
        case sym_lt:    val = BOOL(o1 < o2); break;
        case sym_geq:   val = BOOL(o1 >= o2); break;
        case sym_leq:   val = BOOL(o1 <= o2); break;
        case sym_abs:   val = abs((int)o2); --sp; break;
        case sym_max:   val = o1 > o2 ? o1 : o2; break;
        case sym_min:   val = o1 < o2 ? o1 : o2; break;
        case sym_random:
            --sp;
            if(o2 <= 1){ val = 0; break; }
            srand((int)time(NULL));
            val = rand() % (int)o2;
            break;
        default:
            assert(false); break;
    }
    stack[sp] = val;
}

void evaluate(const expnode *expptr) // --sp by this call
{
    varinfo vinfo;
    const argExpnode *agp;
    if (expptr->prefix == ExpTree) { // expression tree
        oprExpnode *opx = (oprExpnode *)expptr;
        evaluate(opx->operand[0]);
        if (opx->operand[1]) { // binary operators
            evaluate(opx->operand[1]);
            applyOperator(expptr->kind);
        }else { // unary operators
            if (expptr->kind == sym_minus) stack[sp] *= -1;
            else if (expptr->kind == sym_not)
                stack[sp] = BOOL(stack[sp] == 0);
        }
        return;
    }
    switch (expptr->kind) {
        case tok_id: // variables
            vinfo = expptr->v.varinf;
            int idx = vinfo.offset;
            stack[--sp] = vinfo.global ? globals[idx] : stack[localbase - idx];
            break;
        case tok_num: // numbers
            stack[--sp] = expptr->v.intvalue;
            break;
        case sym_func: // functions
            agp = (const argExpnode *)expptr;
            if (sp - agp->count < STACK_LOW)
                abortMessage("stack overflow");
            for (int i = 0; i < agp->count; i++)
                evaluate(agp->args[i]);
            subroutine(agp->index); // --sp by subroutine()
            break;
        case sym_abs:
        case sym_max:
        case sym_min:
        case sym_random:
            agp = (const argExpnode *)expptr;
            if (sp - agp->count < STACK_LOW)
                abortMessage("stack overflow");
            for (int i = 0; i < agp->count; i++)
                evaluate(agp->args[i]);
            applyOperator(expptr->kind);
            break;
        }
        if (expptr->prefix == sym_minus)
            stack[sp] *= -1;
        else if (expptr->prefix == sym_not)
            stack[sp] = BOOL(stack[sp] == 0);
}

void execPrint(const argnode *agp, int count)
{
    int base = sp;
    unsigned char isstring[count];
    for (int i = 0; i < count; i++) {
        expnode *p = agp->p.xlist[i];
        if (p->kind == tok_str) {       // string-literal
            stack[--sp] = p->v.intvalue;
            isstring[i] = true;
        }else {
            evaluate(p);
            isstring[i] = false;
        }
    }
    for (int i = 0; i < count; i++) {
        long t = stack[--base];
        if (isstring[i])
            printf("%s", literalString((int)t));
        else
            printf("%ld", t);
    }
    sp += count;
    if (agp->offset)
        putchar('\n');
}

void execCall(const argnode *agp, int count)
{
    for (int i = 0; i < count; i++)
        evaluate(agp->p.xlist[i]);
    subroutine(agp->offset);
}

void execInput(const argnode *agp, int count)
{
    for (int i = 0; i < count; i++) {
        varinfo vinfo = agp->p.vlist[i];
        int idx = vinfo.offset;
        long *target = vinfo.global ? &globals[idx] : &stack[localbase - idx];
        char str[50];
        char *endptr;
        scanf("%s", str);
        *target = strtol(str, &endptr, 0);
    }
}

/* Duskul version 0.1.1,  2018.03.13,   Takeshi Ogihara, (C) 2018 */
/* Duskul version 1.0.2,  2018.03.25 */
#ifndef __EVALUATE_H__
#define __EVALUATE_H__
#include "stnode.h"
#include "expnode.h"

#define  STACK_LOW      8

/* execute.c */
extern long *stack, *globals;
extern int sp;          // stack pointer
extern int localbase;   // index of 1st local var

void subroutine(int index);
int executeProgram(int mainidx);

/* evaluate.c */
void evaluate(const expnode *expptr);
void execPrint(const argnode *agp, int count);
void execCall(const argnode *agp, int count);
void execInput(const argnode *agp, int count);

#endif /* __EVALUATE_H__ */

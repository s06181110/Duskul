/* Duskul version 0.1.1,  2018.03.13,   Takeshi Ogihara, (C) 2018 */
#if !defined(__EXPRESSION_H__)
#define __EXPRESSION_H__
#include "expnode.h"

/* expression.c */
expnode *expression(void);
expnode *strExpression(void);
void expressionList(expnode *xlist[], int args);

#endif /* !__EXPRESSION_H__ */

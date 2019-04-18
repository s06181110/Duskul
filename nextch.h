/* Duskul version 1.0.0,  2019.03.21,   Takeshi Ogihara, (C) 2018 */
#ifndef NEXTCH_H
#define NEXTCH_H
#include <stdio.h>
#include "commontype.h"

struct textinput; /* an incomplete type */
typedef struct textinput TIN;

extern TIN *stdtxin;

TIN *getTextBuffer(FILE *fp);
void freeTextBuffer(TIN *tip);
void printTextBuffer(FILE *fp, TIN *tip);

int nextbyte(TIN *tip);
int nextch(TIN *tip);
void undoch(int ch, TIN *tip);
bool isEOF(TIN *tip);

#endif /* NEXTCH_H */

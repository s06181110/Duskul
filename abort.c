/* Duskul version 0.1.5,  2018.08.12,   Takeshi Ogihara, (C) 2018 */
#include <stdio.h>
#include <stdlib.h>
#include "getitem.h"
#include "abort.h"

/* defined in errmessages.c */
const char *getErrorMessage(const char *key);

static void abortMessage_string(const char *msg, const char *str)
{
    fprintf(stderr, "ERROR: %s", getErrorMessage(msg));
    if (str)
        fprintf(stderr, " %s", str);
    fputc('\n', stderr);
    if (stdtxin) {
        printTextBuffer(stderr, stdtxin);
        freeTextBuffer(stdtxin);
        stdtxin = NULL;
    }
    exit(1); // === EXIT THE PROGRAM ===
}

void abortMessage(const char *msg) {
    abortMessage_string(msg, NULL);
}

void abortMessageWithToken(const char *msg, const item *s)
{
    char buffer[80];
    itemToString(buffer, s);
    abortMessage_string(msg, buffer);
}

void abortMessageWithString(const char *msg, const char *str)
{
    char buffer[40];
    sprintf(buffer, "'%s'", shorten(str, 32));
    abortMessage_string(msg, buffer);
}

#if defined(DEBUG)
void _assert(int cond, const char *fn, int line)
{
    if (!cond) {
        fprintf(stderr, "Assertion Error: %s (%d)\n", fn, line);
        exit(1);
    }
}
#endif

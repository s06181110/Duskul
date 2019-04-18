/* Duskul version 1.0.0,  2019.03.21,   Takeshi Ogihara, (C) 2018 */
#include <stdlib.h>
#include "nextch.h"
#include "abort.h"

#define MAX_LENGTH_OF_LINE  512

struct textinput {
    FILE *fp;
    unsigned char buf[1024];
    int lines, index, length;
    int ungetch;
};

bool isEOF(TIN *tip) {
    return BOOL(feof(tip->fp));
}

/* extern */
TIN *stdtxin = NULL;

TIN *getTextBuffer(FILE *fp) {
    TIN *tip = malloc(sizeof(TIN));
    tip->fp = fp;
    tip->lines = 0;
    tip->index = 0;
    tip->buf[0] = 0;
    tip->length = 0;
    tip->ungetch = 0;
    return tip;
}

void freeTextBuffer(TIN *tip) {
    fclose(tip->fp);
    free(tip);
}

void printTextBuffer(FILE *fp, TIN *tip) {
    if (!isEOF(tip)) {
        const int leng = 64;
        const char *p = (char *)tip->buf;
        fprintf(fp, "Line#%d: ", tip->lines);
        if (tip->length > leng)
            fprintf(fp, "%s\n", shorten(p, leng));
        else
            fprintf(fp, "%s", p);
        // The last character of a line is a NL(0x0a).
    }
}

static int get_new_line(TIN *tip)
{
    int b, x = 0;
    unsigned char *p = tip->buf;
    while ((b = fgetc(tip->fp)) != EOF) {
        p[x++] = b;
        if (b == '\n')
            break;
        if (x >= MAX_LENGTH_OF_LINE - 1) {
            tip->length = x;
            abortMessage("line too long");
        }
    }
    p[x] = 0;
    tip->length = x;
    if (b == EOF) {
        if (x == 0) { // no char
            tip->ungetch = EOF;
            return EOF;
        }
        p[x] = '\n';
        p[++x] = 0;
        tip->length = x;
    }
    tip->lines++;
    tip->index = 1;
    return tip->buf[0];
}

int nextbyte(TIN *tip) // only used in get_string()
{
    int ch = tip->ungetch;
    if (ch) {
        if (ch != EOF) tip->ungetch = 0;
        return ch;
    }
    ch = tip->buf[tip->index];
    if (ch) {
        tip->index++;
        return ch;
    }
    return get_new_line(tip);
}

int nextch(TIN *tip)
{
    int ch = tip->ungetch;
    if (ch) {
        if (ch == EOF) return EOF;
        tip->ungetch = 0;
    }else {
        ch = tip->buf[tip->index];
        if (ch == 0)
            ch = get_new_line(tip);
        else
            tip->index++;
    }
    if (ch == '/' && tip->buf[tip->index] == '/') { // A comment is found
        tip->index = tip->length;   // Put the index at the end of the line
        return '\n';                // A comment is equivalent to a NL.
    }
    return ch;
}

void undoch(int ch, TIN *tip)
{
    if (tip->ungetch == 0)
        tip->ungetch = ch;
}

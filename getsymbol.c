/* Duskul version 0.1.5,  2018.08.12,   Takeshi Ogihara, (C) 2018 */
#include <stdlib.h>
#include "getsymbol.h"
#include "strliteral.h"
#include "abort.h"

#define STRINGMAX   1024

#define BL  ca_blank
#define SY  (ca_sym | ca_instr)
#define DG  (ca_digit | ca_instr)
#define AL  (ca_alpha | ca_instr)

static unsigned char chTable[256] = {
    0,0,0,0, 0,0,0,0, 0,BL,BL,0, 0,BL,0,0,  // NL & CR
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    (BL|ca_instr),SY,ca_quot,SY, SY,SY,SY,SY, SY,SY,SY,SY, SY,SY,SY,SY,
    DG,DG,DG,DG, DG,DG,DG,DG, DG,DG,SY,SY, SY,SY,SY,SY,
    SY,AL,AL,AL, AL,AL,AL,AL, AL,AL,AL,AL, AL,AL,AL,AL,
    AL,AL,AL,AL, AL,AL,AL,AL, AL,AL,AL,SY, ca_esc, SY, SY, SY,
    SY,AL,AL,AL, AL,AL,AL,AL, AL,AL,AL,AL, AL,AL,AL,AL,
    AL,AL,AL,AL, AL,AL,AL,AL, AL,AL,AL,SY, SY,SY,SY,0,
};
static unsigned char ch2symTable[128] = {
    0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
/* 20 sp   21  !   22  "   23  #   24  $   25  %   26  &   27  ' */
    0, sym_xxx, sym_quo, sym_shp, sym_doll, sym_pcnt, sym_amp, sym_squo,
/* 28  (   29  )   2a  *   2b  +   2c  ,   2d  -   2e  .   2f  / */
    sym_lpar, sym_rpar, sym_ast, sym_plus, sym_comma, sym_minus, sym_dot,
    sym_sls,  0, 0, 0, 0,  0, 0, 0, 0,
/* 30  0   31  1   32  2   33  3   34  4   35  5   36  6   37  7 */
/* 38  8   39  9   3a  :   3b  ;   3c  <   3d  =   3e  >   3f  ? */
    0, 0, sym_col, sym_scol, sym_lt, sym_eq, sym_gt, sym_ques,
/* 40  @   41  A   42  B   ...   57  W */
    sym_at, 0, 0, 0,  0, 0, 0, 0,
    0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
/* 58  X   59  Y   5a  Z   5b  [   5c  \   5d  ]   5e  ^   5f  _ */
    0, 0, 0, sym_lbk, sym_bsls, sym_rbk, sym_xxx, sym_us,
/* 60  `   61  a   62  b   ...   77  w */
    sym_xxx,  0, 0, 0,  0, 0, 0, 0,
    0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,
/* 78  x   79  y   7a  z   7b  {   7c  |   7d  }   7e  ~   7f del */
    0, 0, 0, sym_lbrace, sym_vbar, sym_rbrace, sym_xxx, 0,
};
#define BYTE_IN_STR(x)   (((x) & 0x80) || (chTable[x] & ca_instr))
// Each byte of UTF-8 encoding has the bit of 0x80.

chattr_t chAttribute(int x)
{
    if (x == EOF) return ca_nul;
    return (chattr_t)(chTable[x] & 0x0f);
}

long getnumber(int ch, TIN *tip)
{
    long val = (long)(ch - '0');
    int d = nextch(tip);
    while (chAttribute(d) == ca_digit) {
        val = val * 10 + (long)(d - '0');
        d = nextch(tip);
    }
    if (d != EOF) undoch(d, tip);
    return val;
}

item getsymbol(int ch, TIN *tip)
{
    item s;
    s.token = (token_t)ch2symTable[ch];
    s.kind = ch; // mainly for debugging
    s.a.value = 0;

    if (ch == '<' || ch == '>' || ch == '=') {
        int nx = nextch(tip);
        if (chAttribute(nx) != ca_sym) {
            if (nx != EOF) undoch(nx, tip);
            return s;
        }
        switch (ch) {
            case '<':
                switch (nx) {
                    case '=': s.token = sym_leq; return s;
                    case '>': s.token = sym_neq; return s;
                    default: break;
                }
                break;
            case '>':
                if (nx == '=') { s.token = sym_geq; return s; }
                break;
            case '=':
                if (nx == '=') { s.token = sym_equal; return s; }
                break;
            default: break; // never
        }
        undoch(nx, tip);
    }
    return s;
}

int get_string(TIN *tip)
{
    char buffer[STRINGMAX+4];
    int index = 0;
    int cc;

    cc = nextbyte(tip);
    while (cc != EOF) {
        int attr = chAttribute(cc);
        if (attr == ca_esc) {
            if ((cc = nextbyte(tip)) == EOF)
                abortMessage("unexp EOF");
            switch (cc) {
            case '"':
            case '\\': buffer[index++] = cc; break;
            case 'n': buffer[index++] = '\n'; break;
            case 't': buffer[index++] = '\t'; break;
            default:
                buffer[index++] = '\\';
                buffer[index++] = cc;
                break;
            }
        }else if (attr == ca_quot) {
            buffer[index++] = 0;
            return addLiteralString(buffer);
            // Each string Literal is separately allocated.
        }else if (BYTE_IN_STR(cc)){
            buffer[index++] = cc;
        }else
            abortMessageWithString("illegal string", buffer);
        if (index >= STRINGMAX)
            abortMessageWithString("long string", buffer);
        cc = nextbyte(tip);
    }
    return -1; // Never
}

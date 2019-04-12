/* Duskul version 0.1.5,  2018.08.12,   Takeshi Ogihara, (C) 2018 */

#include "getitem.h"

typedef enum {
    ca_nul = 0, ca_blank = 1, ca_sym, ca_quot, ca_esc,
    ca_digit, ca_alpha,
    ca_instr = 0x10,
} chattr_t;

chattr_t chAttribute(int x);
long getnumber(int ch, TIN *tip);
item getsymbol(int ch, TIN *tip);
int get_string(TIN *tip);

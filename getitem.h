/* Duskul version 0.1.5,  2018.08.12,   Takeshi Ogihara, (C) 2018 */
/* Duskul version 1.0.0,  2019.03.20 */
#if !defined(__GETITEM_H__)
#define __GETITEM_H__

#include <stdio.h>
#include "nextch.h"
#include "token.h"

typedef enum {
    id_resvd, id_global,      // reserved word
    id_static_v, id_local_v, id_param, id_func, id_proc,
    id_undefined, id_new
    // id_new is only used to temporarily keep a table item.
} idkind_t;

struct _idrecord;   // incomplete struct declaration
typedef struct _idrecord idRecord;

struct _item {
    token_t        token;
    unsigned char  kind;    // idkind_t or ASCII char
    short          offset;  // variables, index or funcs/procs,
    union { // associated data. tok_num and tok_str use 'value'
        long       value;   // integer value or index of string-literal
        idRecord   *recptr; // if kind == id_undefined, used in fgetItem()
    } a;
};
typedef struct _item    item;

void ungetItem(item s);
item fgetItem(TIN *tip, bool current_only);
#define getItemLocal()  fgetItem(stdtxin, true)
#define getItem()       fgetItem(stdtxin, false)
bool fgetEOF(TIN *tip);
#define getEOF()        fgetEOF(stdtxin)
#endif /* !__GETITEM_H__ */

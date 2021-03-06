/* Duskul version 0.1.5,  2018.08.12,   Takeshi Ogihara, (C) 2018 */
/* Duskul version 1.0.1,  2019.03.23 */
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "identifiers.h"
#include "idtable.h"
#include "abort.h"

#define GLOBAL_ID_MAX   512
#define HASH_MAX        INT_MAX

static const char * const reserved[] = {
    "and", "break", "call", "declare", "do", "else", "elsif", "end",
    "for", "func", "if", "input", "not", "or", "print", "println",
    "proc", "return", "step", "then", "to", "var", "while", "repeat", "until",
    NULL
};
// Note that the order of strings must be the same as the order of
// the enumerated symbols; sym_and ... sym_while.

static const char *const builtIn[] = {
    "abs", "max", "min", "random", NULL
};

static idtable *resv_table; // 予約語のためのテーブル
static idtable *global_table; //グローバル変数のためのテーブル

const char *reservedWord(int sym) { // reserved配列から予約語の文字列を取って来る
    assert(sym >= reserved_word_0 && sym < all_normal_symbols);
    return reserved[sym - reserved_word_0]; // tokenからオフセットを取った値
}

const char *builtInWord(int sym) {
    assert(sym >= builtIn_word_0 && sym < all_normal_symbols);
    return builtIn[sym - builtIn_word_0]; // tokenからオフセットを取った値
}

void idtablesInitialize(void) // idtableの初期化
{
    int cap = 2 * sizeof(reserved) / sizeof(const char *); // 何かメモリを確保している
    resv_table = idtableCreate(cap, false); // 予約語のテーブルを作成
    int sym = reserved_word_0;
    for (int x = 0; reserved[x] != NULL; x++) {
        struct string_info strp = makeStringInfo(reserved[x], true);
        idRecord *rec = idtableAdd(resv_table, &strp);
        rec->kind = id_resvd;
        rec->order = sym++;
    }
	global_table = idtableCreate(GLOBAL_ID_MAX, true); // グローバル?のテーブルを作成
	sym = builtIn_word_0;
    for (int x = 0; builtIn[x] != NULL; x++) {
        struct string_info strp = makeStringInfo(builtIn[x], true);
        idRecord *rec = idtableAdd(global_table, &strp);
        rec->kind = id_func;
        rec->order = x;
        int params = 1;
        if (!strcmp(builtIn[x], "min") || !strcmp(builtIn[x], "max")) params = 2;
        addBuiltIn(rec, params);
    }
    srand((int)time(NULL)); // 組み込み関数のランダムのための初期化
}

void idtablesFree(void) // メモリの解放
{
    idtableFree(resv_table);
    idtableFree(global_table);
}

struct string_info makeStringInfo(const char *str, bool newid)
{
    struct string_info idt;
    const unsigned char *ustr = (unsigned char *)str;
    int x, ch;
    unsigned int v;
    idt.to_be_defined = newid;
    idt.name = str;
    if ((v = ustr[0]) == 0) {
        idt.length = 0;
    }else if ((ch = ustr[1]) == 0) {
        idt.length = 1;
    }else { // hash value
        v = (v * 148) ^ ch;
        for (x = 2; (ch = ustr[x]) != 0; x++)
            v = ((v * 9) & HASH_MAX) ^ ch;
        idt.length = x;
    }
    idt.hashvalue = v;
    return idt;
}

item identifierSearch(stringInfoPtr strp)
{
    item t = { 0 };
    idRecord *rec = idtableSearch(resv_table, strp);
    if (rec->str != NULL) {
        t.token = rec->order;
        return t;
    }
    t.token = tok_id;
    if (strp->to_be_defined) { // should be new id
        rec = (display_index == 0) // top level
            ? idtableSearch(global_table, strp)
            : searchLocalIdentifier(strp);
    }else {
        rec = searchLocalIdentifier(strp);
        if (rec == NULL)
            rec = idtableSearch(global_table, strp);
    }
    if (rec && rec->str) {
        t.kind = rec->kind;
        t.offset = rec->order;
        t.a.recptr = rec;
        return t;
    }
    t.kind = id_undefined;
    return t;
}

idRecord *addIdentifier(stringInfoPtr strp) {
    if (display_index == 0) // top level
        return idtableAdd(global_table, strp);
    return addLocalIdentifier(strp);
}

/* Duskul version 0.1.5,  2018.08.12,   Takeshi Ogihara, (C) 2018 */
/* Duskul version 1.0.1,  2019.03.23 */
#if !defined(__IDENTIFIERS_H__)
#define __IDENTIFIERS_H__

#include "getitem.h"

#define MAX_IDENTIFIER_LENGTH   127     // must be less than 255

struct _idtable;
typedef struct _idtable     idtable;

struct string_info {
    int            hashvalue;
    unsigned char  length;
    unsigned char  to_be_defined;
    const char     *name;
};
typedef const struct string_info *stringInfoPtr;

struct _idrecord {
    unsigned char  kind;    // idkind_t
    unsigned char  length;  // length of identifier
    short          order;   // index of variable, func, or proc
    // If the table is for reserved words, 'order' has the value of token_t.
    const char     *str;
};

extern int currentLocalOffset;
extern int maxLocalOffset;

const char *reservedWord(int sym);

void addBuiltIn(idRecord *idp, int params);
void idtablesInitialize(void);
void idtablesFree(void);
struct string_info makeStringInfo(const char *str, bool newid);
item identifierSearch(stringInfoPtr strp);
idRecord *addIdentifier(stringInfoPtr strp);

void resetLocalList(void);
void blockNestPush(void);
void blockNestPop(void);

#endif /* !__IDENTIFIERS_H__ */

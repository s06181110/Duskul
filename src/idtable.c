/* Duskul version 0.1.5,  2018.08.12,   Takeshi Ogihara, (C) 2018 */
/* Duskul version 1.0.1,  2019.03.23 */
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "identifiers.h"
#include "idtable.h"
#include "abort.h"

#define STRPOOLSZ(x)    ((x) * 4)  // estimate needed size for all string
#define MAX_LOCAL_IDS   512
#define MAX_LOCAL_NESTS 24

/* extern */
int currentLocalOffset;
int maxLocalOffset;

struct string_pool {
    int strcapacity;
    int strindex;
    char pool[4];   // Defined identifiers (var, func, or proc) are saved.
};

struct _idtable {   // hash table
    int capacity;
    int entries;
    struct string_pool *strpool;
    idRecord *records;
};

static idRecord local_identifiers[MAX_LOCAL_IDS];
static int local_id_index = 0;

static struct {
    int idrec_index;
    int str_index;
    int local_offset;
} local_display[MAX_LOCAL_NESTS];
/* extern */ int display_index = 0;
static struct string_pool *local_pool = NULL;

static struct string_pool *make_string_pool(int capacity)
{
    size_t sz = sizeof(struct string_pool) + capacity;
    struct string_pool *ptr = malloc(sz);
    ptr->strindex = 0;
    ptr->strcapacity = capacity;
    return ptr;
}

static const char *duplicate_str(struct string_pool *pp, stringInfoPtr strp)
{
    int length = strp->length + 1;
    if (pp->strindex + length >= pp->strcapacity)
        abortMessage("many ident"); // ERROR
    char *headp = pp->pool + pp->strindex;
    strcpy(headp, strp->name);
    pp->strindex += length;
    return headp;
}

idtable *idtableCreate(int capacity, bool dupflag)
{
    idtable *p = malloc(sizeof(idtable));
    p->capacity = capacity;
    p->entries = 0;
    p->records = calloc(p->capacity, sizeof(idRecord));
    p->strpool = dupflag ? make_string_pool(STRPOOLSZ(capacity)) : NULL;
    return p;
}

void idtableFree(idtable *tabp) {
    if (tabp == NULL) return;
    if (tabp->strpool) free(tabp->strpool);
    free(tabp->records);
    free(tabp);
}

/// Searches the table 'tabp' for an identifier 'name'.
/// If the identifier is unknown, the member 'str' == NULL.
idRecord *idtableSearch(idtable *tabp, stringInfoPtr strp)
{
    int h = strp->hashvalue % tabp->capacity;
    int len = strp->length;
    const char *name = strp->name;
    idRecord *recs = tabp->records;
    for ( ; ; ) {
        const char *str = recs[h].str;
        if (str == NULL) // not found
            break;
        if (recs[h].length == len && strcmp(str, name) == 0)
            break; // found successfully
        h = (h + 17) % tabp->capacity;  // 17 is a prime number.
    }
    return &recs[h];
}

idRecord *idtableAdd(idtable *tabp, stringInfoPtr strp)
{
    if (tabp->entries + 1 >= tabp->capacity)
        abortMessage("many ident"); // ERROR
    idRecord *rec = idtableSearch(tabp, strp);
    if (rec->str == NULL) { // empty entry found
        rec->str = (tabp->strpool == NULL) ? strp->name
            : duplicate_str(tabp->strpool, strp);
        rec->length = strp->length;
        tabp->entries++;
        return rec;
    }
    return NULL; // the same name exists.
}

void resetLocalList(void)
{
    if (local_pool == NULL)
        local_pool = make_string_pool(MAX_LOCAL_IDS);
    local_pool->strindex = 0;
    local_id_index = 0;
    display_index = 0;  // top level
    currentLocalOffset = 0;
}

/// Add a local block
void blockNestPush(void)
{
    if (display_index >= MAX_LOCAL_NESTS)
        abortMessage("many nest"); // ERROR
    local_display[display_index].idrec_index = local_id_index;
    local_display[display_index].str_index = local_pool->strindex;
    local_display[display_index].local_offset = currentLocalOffset;
    display_index++;
}

/// Remove a local block
void blockNestPop(void)
{
    assert(display_index > 0);
    display_index--;
    local_id_index = local_display[display_index].idrec_index;
    local_pool->strindex = local_display[display_index].str_index;
    if (maxLocalOffset < currentLocalOffset)
        maxLocalOffset = currentLocalOffset;
    currentLocalOffset = local_display[display_index].local_offset;
}

idRecord *searchLocalIdentifier(stringInfoPtr strp)
{
    assert(display_index > 0);
    int index = local_id_index - 1;
    int limit = strp->to_be_defined
        ? local_display[display_index - 1].idrec_index : 0;
    int len = strp->length;
    const char *name = strp->name;
    while (index >= limit) {
        if (local_identifiers[index].length == len
                && strcmp(local_identifiers[index].str, name) == 0)
            return &local_identifiers[index];
        index--;
    }
    return NULL;
}

idRecord *addLocalIdentifier(stringInfoPtr strp)
{
    assert(display_index > 0);
    idRecord *idrec = &local_identifiers[local_id_index];
    idrec->str = duplicate_str(local_pool, strp);
    idrec->length = strp->length;
    local_id_index++;
    return idrec;
}

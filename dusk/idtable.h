/* Duskul version 0.1.5,  2018.08.12,   Takeshi Ogihara, (C) 2018 */
/* Duskul version 1.0.1,  2019.03.23 */
#if !defined(__IDTABLE_H__)
#define __IDTABLE_H__

extern int display_index;   // if 0, parsing top level

idtable *idtableCreate(int capacity, bool dupflag);
void idtableFree(idtable *tabp);
idRecord *idtableSearch(idtable *tabp, stringInfoPtr strp);
idRecord *idtableAdd(idtable *tabp, stringInfoPtr strp);
idRecord *searchLocalIdentifier(stringInfoPtr strp);
idRecord *addLocalIdentifier(stringInfoPtr strp);

#endif /* !__IDTABLE_H__ */

/* Duskul version 0.1.5,  2018.08.12,   Takeshi Ogihara, (C) 2018 */
#if !defined(__ABORT_H__)
#define __ABORT_H__

struct _item;
//各関数の宣言を行うことでコンパイラに関数の情報を教える
void abortMessage(const char *msg);
void abortMessageWithToken(const char *msg, const struct _item *s);
void abortMessageWithString(const char *msg, const char *str);

/* printitem.c */
const char *shorten(const char *s, int limit);
void itemToString(char *buffer, const struct _item *p);
#if defined(DEBUG)
void printItem(const struct _item *p);
#endif

#if defined(DEBUG)
void _assert(int cond, const char *fn, int line);
#  define   assert(c)   _assert((c), __FILE__, __LINE__)
#else
#  define   assert(c)   // ignored
#endif

#endif /* !__ABORT_H__ */

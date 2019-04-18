/* Duskul version 0.1.2,  2018.05.13,   Takeshi Ogihara, (C) 2018 */
#if !defined(__COMMONTYPE_H__)
#define __COMMONTYPE_H__
#include <limits.h>

/* Note that it may conflict with the implementation of C (in the future) */
#if !defined(true)
# define true     1
# define false    0
# define bool     int
#endif

/* "True" in the conditional expression of C is not necessarily 1.
   So, don't describe as "expression == TRUE" for comparation. */
#define BOOL(c)   ((c) ? true : false)

#endif /* !__COMMONTYPE_H__ */

#ifndef MY_DEBUG_H_
#define MY_DEBUG_H_

/* First include stdio.h, which may contain the prototype for the external dprintf.
 * We do not want that. We redefine dprintf to our local implementation. */
#include <stdio.h>
#define dprintf my_dprintf

extern void my_dprintf (int, const char *, const char *, ...);

#endif /* MY_DEBUG_H_ */

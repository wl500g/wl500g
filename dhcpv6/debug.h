#ifndef MY_DEBUG_H_
#define MY_DEBUG_H_

/* First include stdio.h, which may contain the prototype for the external dprintf.
 * We do not want that. We redefine dprintf to our local implementation. */
#include <stdio.h>

#define log_error(ARGS...)	my_dprintf(LOG_ERR, FNAME, ##ARGS)
#define log_warning(ARGS...)	my_dprintf(LOG_WARNING, FNAME, ##ARGS)
#define log_notice(ARGS...)	my_dprintf(LOG_NOTICE, FNAME, ##ARGS)
#define log_info(ARGS...)	my_dprintf(LOG_INFO, FNAME, ##ARGS)
#ifdef DHCP6_DEBUG
# define log_debug(ARGS...)	my_dprintf(LOG_DEBUG, FNAME, ##ARGS)
#else
# define log_debug(ARGS...)
#endif

extern void my_dprintf (int, const char *, const char *, ...);

#endif /* MY_DEBUG_H_ */

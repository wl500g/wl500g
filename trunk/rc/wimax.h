#ifndef _WIMAX_H_100308_
#define _WIMAX_H_100308_

#ifdef __CONFIG_MADWIMAX__
int madwimax_start( char * ifname );
int madwimax_check(void);
int madwimax_main(int argc, char **argv);
void get_wimax_ifname( char * out, int unit );
#endif

#endif

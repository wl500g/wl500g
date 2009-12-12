#include <stdio.h>

extern int start_switch(void);
extern int start_vlan(void);
extern int stop_vlan(void);
#ifdef CONFIG_PPTP_CLIENT
extern int start_pptp();
extern int stop_pptp();
#endif
#ifdef CONFIG_IPSEC_FREESWAN
extern int start_ipsec();
extern int stop_ipsec();
#endif

void extra_restore_defaults()
{
}



void rc1_start() 
{
	setup_switch(); 
	start_vlan();
}
void rc1_stop()
{
	stop_vlan();
}


void rc7_start()
{
#ifdef CONFIG_PPTP_CLIENT
	start_pptp();
#endif
#ifdef CONFIG_IPSEC_FREESWAN
	start_ipsec();
#endif
}


void rc7_stop()
{
#ifdef CONFIG_IPSEC_FREESWAN
	stop_ipsec();
#endif
#ifdef CONFIG_PPTP_CLIENT
	stop_pptp();
#endif
}




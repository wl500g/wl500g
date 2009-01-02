/* module to include the modules relavent to the mib-II mib(s) */
config_require(mibII/system_mib)
config_require(mibII/at)
config_require(mibII/interfaces)
config_require(mibII/tcp)
config_require(mibII/icmp)
config_require(mibII/ip)
config_require(mibII/udp)

void init_mibII (void);
  

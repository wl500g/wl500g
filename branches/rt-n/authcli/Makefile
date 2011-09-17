all: authcli.c authd2.h sha1.c
	cc -o authcli authcli.c sha1.c globals.c

install: all
	cp authcli /usr/local/sbin/
	cp authcli.sh /usr/local/etc/rc.d/

	@echo ""
	@echo "************************************************************************"
	@echo "*                                                                      *"
	@echo "* Please, copy your authcliw.conf to /usr/local/etc/ and start authcli *"
	@echo "* using /usr/local/etc/rc.d/authcli.sh start. For stoping authcli use  *"
	@echo "* /usr/local/etc/rc.d/authcli.sh stop                                  *"
	@echo "*                                                                      *"
	@echo "* You may also compile authcli with next options which you can change  *"
	@echo "* in authd2.h:                                                         *"
	@echo "*    DEBUG             - with debug mode                               *"
	@echo "*    DEFAULT_PID_PATH  - Path for pid file. Default                    *"
	@echo "*                        /var/run/authcli.pid. If you change it you    *"
	@echo "*                        must chang and /usr/local/etc/rc.d/authcli.sh *"
	@echo "*    DEFAULT_CONF_PATH - Path to authcliw.conf                         *"
	@echo "*                        Default /usr/local/etc/authcliw.conf          *"
	@echo "*                                                                      *"
	@echo "*                                                                      *"
	@echo "************************************************************************"
	@echo ""

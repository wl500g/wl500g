#!/bin/sh


case "$1" in
start)
	if test -e /var/run/authcli.pid
	then
	  pid=`cat /var/run/authcli.pid`
	  res=`ps -p $pid|grep authcli`
	  if test -n "$res"
	  then
		echo ' authcli alredy is started.'
		echo ' If you think that authcli is stoped, please, remove the /var/run/authcli.pid '
	  else
        	echo -n 'authcli '
        	/usr/local/sbin/authcli &
	  fi
	else
        	echo -n 'authcli '
        	/usr/local/sbin/authcli &
	fi
	;;


stop)
	if test -e /var/run/authcli.pid
	then
        	echo 'Stop authcli.'
	        kill `cat /var/run/authcli.pid`
		rm -f /var/run/authcli.pid
	else
		echo 'autchcli alredy is stoped '
		echo 'If you think that authcli is running, please, make killall authcli.'
	fi
	;;

*)
	echo "Usage:  { start | stop  }" 2>&1
	exit 65
	;;
esac




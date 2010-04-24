#!/bin/sh

PID=$1
IFNAME=$2
TARGET=$3
KILL=`nvram get wan_wimax_restart`
LOG=`nvram get wan_wimax_syslog`
DAEMON=`nvram get wan_proto_t`
POST_FN="/usr/local/sbin/post-check-connection"

if [ -z "$IFNAME" ] || [ -z "$TARGET" ] || [ "$TARGET" == "0.0.0.0" ]; then
    if [ "$LOG" == "1" ] ;  then
	logger -t $DAEMON "kill madwimax pid=$PID ($IFNAME $TARGET)"
    fi
    if [ "$KILL" == "1" ] ;  then
	kill $PID
    fi
    if [ -e $POST_FN ]; then
	$POST_FN $IFNAME
    fi
fi

if [ "$LOG" == "1" ] ;  then
	logger -t $DAEMON "ping to $TARGET via $IFNAME"
fi

PACKETS=5
PING_FN="/var/state/$IFNAME_ping.log"
ping -q -I $IFNAME -c $PACKETS $TARGET 2>/dev/null >$PING_FN

RET=`awk '/packets received/ {print $4}' $PING_FN`
RTT=`awk '/min/ {print $4 $5}' $PING_FN`
nvram set wan_wimax_ping_t="$RTT"

if [ -z $RET ] || [ $RET -eq 0 ] ; then
    if [ "$KILL" == "1" ] ;  then
	if [ "$LOG" == "1" ] ;  then
	    logger -t $DAEMON "ping to $TARGET via $IFNAME failed. kill madwimax pid=$PID"
	fi
	kill $PID
    fi
    if [ -e $POST_FN ]; then
	$POST_FN $IFNAME
    fi
fi

#!/bin/sh

PID=$1
IFNAME=$2
TARGET=$3
KILL=`nvram get wmx_chk_rst`
LOG=`nvram get wmx_chk_log`
POST_FN="/usr/local/sbin/post-check-connection"

if [ "$IFNAME" == "" ] || [ "$TARGET" == ""  ] ; then
    if [ "$LOG" == "1" ] ;  then
	logger "[wmx_checker] kill madwimax pid=$PID ( $IFNAME $TARGET ) "
    fi
    if [ "$KILL" == "1" ] ;  then
	kill $PID
    fi
    if [ -e $POST_FN ]; then
	$POST_FN $IFNAME
    fi
fi

if [ "$LOG" == "1" ] ;  then
	logger "[wmx_checker] ping to $TARGET via $IFNAME"
fi

PACKETS=5
PING_FN=/var/state/$IFNAME_ping.log
ping -q -I $IFNAME -c $PACKETS $TARGET 2>/dev/null >$PING_FN

RET=`awk '/packets received/ {print $4}' $PING_FN`
RTT=`awk '/min/ {print $4 $5}' $PING_FN`
nvram set wmx_ping_t="$RTT"

if [ $RET -eq 0 ] ; then
    if [ "$KILL" == "1" ] ;  then
	if [ "$LOG" == "1" ] ;  then
	    logger "[wmx_checker] ping to $TARGET via $IFNAME failed. kill madwimax pid=$PID"
	fi
	kill $PID
    fi
    if [ -e $POST_FN ]; then
	$POST_FN $IFNAME
    fi
fi

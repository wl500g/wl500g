#!/bin/sh

PID=$1
IFNAME=$2
TARGET=$3
if [ "$IFNAME" == "" ] || [ "$TARGET" == ""  ] ; then
   kill $PID
fi

PACKETS=5
PING_FN=/var/state/$IFNAME_ping.log
ping -q -I $IFNAME -c $PACKETS $TARGET 2>/dev/null >$PING_FN

RET=`awk '/packets received/ {print $4}' $PING_FN`
RTT=`awk '/min/ {print $4 $5}' $PING_FN`
nvram set wmx_ping_t="$RTT"
if [ $RET -eq 0 ] ; then
   kill $PID
fi

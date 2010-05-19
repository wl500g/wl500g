/usr/sbin/chat -s -S -V -t 3 -f $1 2>/tmp/chat.log
E=$?
awk -f /usr/ppp/cdma/GTRAN/cdmastat.awk /tmp/chat.log >/tmp/gtran.log 2>&1
exit $E

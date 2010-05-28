/usr/sbin/chat -s -S -V -t 15 -f $1 2>/tmp/chat.log
E=$?
awk -f /tmp/ppp/ipw/ipwstat.awk /tmp/chat.log >/tmp/ipw.log 2>&1

if [ $E == 0 ]; then
 nvram set ipw_stat_connect=Připojeno
else
 nvram set ipw_stat_connect=Nepřipojeno
fi

nvram set ipw_stat_signal="$(grep Signal /tmp/ipw.log|sed s/.*:\ //)"
nvram set ipw_stat_battery="$(grep Battery /tmp/ipw.log|sed s/.*:\ //)"

if [ $E == 0 ]; then
 if [ "$(grep "+COPS: 0,0," /tmp/chat.log)" == "" ]; then
  nvram set ipw_stat_operator="T-Mobile CZ 4G"
 else
  nvram set ipw_stat_operator="$(grep Operator /tmp/ipw.log|sed s/.*:\ //) GPRS/EDGE"
 fi
fi

if [ "$(nvram get ipw_stat_battery)" == "" ]; then
 nvram set ipw_stat_battery=---
 nvram set ipw_stat_present="Nalezen 4G/GPRS/EDGE modem"
 nvram set ipw_stat_version=V2
else
 nvram set ipw_stat_present="Nalezen 4G modem"
 nvram set ipw_stat_version=V1
fi

PINOK=$(sed -n '/CPIN=/{n;p}' /tmp/chat.log)
USIM=$(sed -n '/CPIN:/{s/.*USIM //g;p}' /tmp/chat.log)
SIMLOCK=$(sed -n '/CLCK:/{s/.*CLCK: //g;s/,.*//g;p};' /tmp/chat.log)

if [ "$USIM" == "PIN" -o "$SIMLOCK" == "1" ]; then 
 nvram set ipw_stat_pin_lock=1
 SIMLOCK=1
else
 nvram set ipw_stat_pin_lock=0
 SIMLOCK=0
fi

# Pri spatnem zadani PINu se PIN vymaze a uz se neposila
# kvuli moznemu zablokovani SIM po trech pokusech

if [ "$SIMLOCK" == "1" -a "$PINOK" == "ERROR" ]; then
 nvram set ipw_stat_connect="Chybný PIN"
 nvram set ipw_stat_pin="Předchozí zadaní PIN SIM karty T-Mobile ($(nvram get ipw_pin)) bylo chybné"
 nvram set ipw_pin=
 nvram commit
 /usr/ppp/ipw/update
fi

# Pokud je PIN potreba tak reportovat
if [ "$SIMLOCK" == "1" -a "$(nvram get ipw_pin)" == "" ]; then
 nvram set ipw_stat_connect="Je vyžadován PIN"
 nvram set ipw_stat_pin="Prosím zadejte PIN SIM karty T-Mobile"
fi

exit $E

echo '
<pre>
<h3>Status Information</h3>'
cat /proc/version
cat /.version
echo
date
uptime

echo '
<hr>
<h3>Interfaces</h3>'
ifconfig

echo '
<hr>
<h3>Routing Table</h3>'
route -n

echo '
<hr>
<h3>ARP Table</h3>'
cat /proc/net/arp

echo '
<hr>
<h3>IP Tables</h3>'
iptables -L -vn

echo '
<hr>
<h3>IP Tables NAT</h3>'
iptables -L -t nat -vn

if [ "$(nvram get lan_dhcp)" == "1" ]; then
 echo '
 <hr>
 <h3>DHCP leases</h3>'
 killall -HUP dnsmasq
 sleep 1
 cat /tmp/dnsmasq.log
fi 

echo '
<hr>
<h3>Mount points</h3>'
mount

echo '
<hr>
<h3>Filesystem info</h3>'
df

echo '
<hr>
<h3>Partitions</h3>'
cat /proc/partitions

echo '
<hr>
<h3>USB HDD info</h3>'
cat /proc/scsi/scsi|awk '
 BEGIN {getline; print $0;i=0}
 {
 print ""
 print $0
 getline; print $0
 getline; print $0
 system("cat /proc/scsi/usb-storage-"i"/"i)
 i++
 }'

echo '
<hr>
<h3>Process List</h3>'
ps

echo '
<hr>
<h3>Kernel Modules</h3>'
cat /proc/modules

echo '
<hr>
<h3>USB Devices</h3>'
cat /proc/bus/usb/devices	

echo '
<hr>
<h3>USB devfs Devices</h3>'
cat /proc/bus/usb/devpath	

if [ -f /tmp/chat.log ]; then
echo '
<hr>
<h3>PPP chat log</h3>'
cat /tmp/chat.log	
fi

if [ "$(nvram get boot_local)" = "$(cat /.version)" ]; then
echo '
<hr>
<h3>FlashFS file list</h3>'
flashfs list
fi
 
echo '
<hr>
<h3>CPU Info</h3>'
cat /proc/cpuinfo

echo '
<hr>
<h3>Memory</h3>'
cat /proc/meminfo


echo '</pre>'
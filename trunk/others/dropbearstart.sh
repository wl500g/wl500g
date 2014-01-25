#! /bin/sh
#
# Copyright (C) 2009 by Alexander I. Buzin <al37919@gmail.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA


# This script is intended to be run automatically during boot up.
# It ensures that dropbear keys are in the right place either by
# extracting them from flashfs if it is disabled or by by generating
# them and saving in flashfs if they are absent at all.
# IMPORTANT: This script does not change the state of flashfs!

SSHD_ETC_LOC=tmp/local/etc/dropbear
SSHD_ETC=/$SSHD_ETC_LOC

PATH=/sbin:/usr/sbin:/bin:/usr/bin
export PATH

MTD="/dev/mtd/4"
MTDBLOCK="/dev/mtd/block4"

check_keys() {
#check if the all of key files are in the right place
if ! [ -s $SSHD_ETC/dropbear_rsa_host_key -a \
       -s $SSHD_ETC/dropbear_dss_host_key -a \
       -s $SSHD_ETC/dropbear_ecdsa_host_key ]; then
	return 0;
else
	return 1;
fi
}

generate_key() {
if ! [ -s $1 ]; then
	dropbearkey -t $keytype -f $1 | /bin/grep "$keytype" > $1.pub
fi
}

if check_keys; then
	mkdir -p $SSHD_ETC
	#check if the key files are stored in flashfs
	if [ -n "$(/bin/tar -tzf $MTDBLOCK 2> /dev/null | \
	     /bin/grep -E $SSHD_ETC_LOC/dropbear_\(ecdsa\|rsa\|dss\){1}_host_key)" ]; then
		/bin/tar -C / -xzf $MTDBLOCK $SSHD_ETC_LOC tmp/local/etc/ssh.*
	fi
	if check_keys; then
		#generate new keys
		for keytype in rsa dss ecdsa; do
			generate_key "$SSHD_ETC/dropbear_${keytype}_host_key"
		done

		#store generated keys in the flashfs for future use
		TMP_B=/tmp/_tmp
		mkdir -p $TMP_B/tmp/local/etc
		/bin/tar -C $TMP_B/ -xzf $MTDBLOCK 2> /dev/null
		cp -r $SSHD_ETC $TMP_B/tmp/local/etc
		/bin/tar -C $TMP_B/ -czf /tmp/_flash.tar.gz etc tmp

		/sbin/flash /tmp/_flash.tar.gz $MTD
		rm -r $TMP_B /tmp/_flash.tar.gz
	fi
fi

dropbear "$@"

exit $?

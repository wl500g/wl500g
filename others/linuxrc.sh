#!/bin/sh
#
# Copyright (C) 2004, 2005 by Oleg I. Vdovikin <oleg@cs.msu.su>
#
# 2011-09-15 - modifications for 2.6 kernels by Leonid Lisovskiy <lly@sf.net>
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
#

# spin-up hdd
/sbin/poweron

# get the boot device
boot_dev=$(nvram get boot_dev)

# short-cut
if [ -z "$boot_dev" ]; then
	exec /sbin/init <dev/null >dev/null 2>&1
fi

# /proc
mount -t proc proc /proc
# disable hotplug
echo > /proc/sys/kernel/hotplug

# prepare /dev & /sys
mount -t sysfs sysfs /sys
mount -t tmpfs -o noatime,size=200K devfs /dev
mknod -m 0600 /dev/console c 5 1
mknod -m 0666 /dev/null c 1 3
mknod -m 0640 /dev/nvram c 253 0
# coldplug udev
/sbin/mdev -s
ln -sf /proc/self/fd /dev/fd
ln -sf /proc/self/fd/0 /dev/stdin
ln -sf /proc/self/fd/1 /dev/stdout
ln -sf /proc/self/fd/2 /dev/stderr

# set hotplug to mdev
echo /sbin/mdev > /proc/sys/kernel/hotplug

# ide modules for WL700g
if [ "$(nvram get boardtype)" = "0x042f" -a "$(nvram get boardnum)" = "44" ]; then
	WL700=yes
	insmod ide-core && insmod aec62xx && insmod ide-detect && insmod ide-disk
fi

# usb modules
if [ ! -b "$boot_dev" ]; then
	insmod usbcore
	usb21=$(nvram get usb20_disable_x)
	[ -z "$usb21"] && usb21="0"
	[ "$usb21" -ne 1 ] && insmod ehci-hcd
	[ "$usb21" -ne 2 ] && (insmod ohci-hcd; insmod uhci-hcd)
	sleep 2s
	insmod scsi_mod && insmod sd_mod && insmod usb-storage
	# wait for initialization to complete
	while [ ! -d /sys/module/usb_storage ]; do
		sleep 1s
	done
fi

# wait for disc appear, max 15 sec
i=0
while [ -z "$(ls /sys/class/scsi_disk/)" -a -z "$(ls /sys/bus/ide/devices/ 2>/dev/null)" ]; do
	sleep 1s
	i=$((i + 1))
	if [ $i -gt 15 ]; then
		break
	fi
done

# trying to mount new rootfs
for fstyp in ext3 ext2; do
	mount -t $fstyp -o ro "$boot_dev" /mnt
	if [ $? -eq 0 ]; then
		break
	fi
done

# enable hotplug
echo /sbin/hotplug > /proc/sys/kernel/hotplug

# switch root
if [ -d /mnt/mnt ]; then
	cd /mnt
	pivot_root . mnt
	umount /mnt/dev
	umount /mnt/sys
	umount /mnt/proc
	exec chroot . sbin/init <dev/null >dev/null 2>&1
fi

# if something fails...
cd /

# unmount everything
umount /mnt

# remove modules
rmmod usb-storage && rmmod sd_mod && rmmod scsi_mod
rmmod uhci-hcd
rmmod ohci-hcd
rmmod ehci-hcd
rmmod usbcore
[ ! -z "$WL700" ] && rmmod ide-disk && rmmod ide-detect && rmmod aec62xx && rmmod ide-core

umount /dev
umount /sys
umount /proc

# finally execute init
exec /sbin/init <dev/null >dev/null 2>&1

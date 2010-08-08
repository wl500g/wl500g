#!/bin/sh
#
# Copyright (C) 2004, 2005 by Oleg I. Vdovikin <oleg@cs.msu.su>
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

# mount procfs
mount -t proc none /proc

# disable hotplug
echo > /proc/sys/kernel/hotplug

# insert ide modules first
insmod ide-mod && insmod ide-probe-mod && insmod ide-disk

# insert usb modules
if [ ! -b "$boot_dev" ]; then
	insmod usbcore
	[ "$(nvram get usb20_disable_x)" -ne 2 ] && (insmod usb-ohci; insmod usb-uhci)
	[ "$(nvram get usb20_disable_x)" -ne 1 ] && insmod ehci-hcd
	sleep 2s
	insmod scsi_mod && insmod sd_mod && insmod usb-storage
	# wait for initialization to complete
	sleep 2s
fi

# trying to mount new rootfs if it exists
mount -t ext3 -o ro "$boot_dev" /mnt || mount -t ext2 -o ro "$boot_dev" /mnt

# enable hotplug
echo /sbin/hotplug > /proc/sys/kernel/hotplug

if [ -d /mnt/mnt ]; then
	cd /mnt
	mount -t devfs none dev
	pivot_root . mnt
	exec chroot . sbin/init <dev/null >dev/null 2>&1
fi

# if something fails
cd /

# unmount everything
umount /mnt

mount -t devfs none dev

# remove modules
rmmod usb-storage && rmmod sd_mod && rmmod scsi_mod
rmmod ehci-hcd
rmmod usb-uhci
rmmod usb-ohci
rmmod usbcore
rmmod ide-disk && rmmod ide-probe-mod && rmmod ide-mod

umount /proc

# finally execute init
exec /sbin/init <dev/null >dev/null 2>&1

#!/bin/sh
#
# Copyright (C) 2004 by Oleg I. Vdovikin <oleg@cs.msu.su>
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

PATH=/sbin:/usr/sbin:/bin:/usr/bin
export PATH

VERSION="$(cat /.version)"; VERSION=${VERSION%-r[0-9]*}
MTD="/dev/mtd/4"
MTDBLOCK="/dev/mtd/block4"

case "$1" in
	status)
		if [ "$(nvram get boot_local)" = "$VERSION" ] ||
		   [ "$(nvram get boot_local)" = "enabled" ]; then
			echo "boot_local enabled"
		else 
			echo "boot_local disabled"
		fi
		;;
	start)
		if [ "$(nvram get boot_local)" = "$VERSION" ] ||
		   [ "$(nvram get boot_local)" = "enabled" ]; then
			/bin/tar -C / -xzf $MTDBLOCK
		fi
		;;
	enable)
		if [ "$(nvram get boot_local)" != "enabled" ]; then
			nvram set boot_local="$VERSION"
			nvram commit
		fi
		;;
	enabled)
		if [ "$(nvram get boot_local)" != "enabled" ]; then
			nvram set boot_local=enabled
			nvram commit
		fi
		;;
	disable)
		nvram set boot_local
		nvram commit
		;;
	clear)
		/sbin/erase $MTD
		;;
	load)
		/bin/tar -C / -xzvf $MTDBLOCK
		;;
	list)
		/bin/tar -C / -tzf $MTDBLOCK
		;;
	save)
		[ -f /usr/local/.files ] && FILES=$(/bin/grep -v ^# /usr/local/.files)
		/bin/tar -C / -czvf /tmp/flash.tar.gz /tmp/local $FILES && 
		ls -l /tmp/flash.tar.gz &&
		echo "Check saved image and type \"$0 commit\" to commit changes"
		;;
	commit)
		/sbin/flash /tmp/flash.tar.gz $MTD &&
		rm -f /tmp/flash.tar.gz &&
		echo "Committed."
		;;
	*)
		echo "Usage: $0 status|start|enable|enabled|disable|clear|load|list|save|commit"
		;;
esac

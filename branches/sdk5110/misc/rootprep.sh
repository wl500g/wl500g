#!/bin/bash
#
# Miscellaneous steps to prepare the root filesystem
#
# Copyright 2004, Broadcom Corporation
# All Rights Reserved.
# 
# THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
# KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
# SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
#
# $Id$
#

ROOTDIR=$PWD

# tmp
mkdir -p tmp
mkdir -p tmp/var
ln -sf tmp/var var
(cd $ROOTDIR/usr && ln -sf ../tmp)

# dev
mkdir -p dev

# miscellaneous
mkdir -p sys
mkdir -p mnt
mkdir -p opt
mkdir -p home
mkdir -p proc

# usr/local -> tmp/local
ln -sf ../tmp/local usr/local

# etc
mkdir -p usr/etc
mkdir usr/etc/crontabs
mkdir usr/etc/l7-protocols
ln -sf ../usr/etc tmp/etc
ln -sf tmp/etc etc
echo "/lib" > etc/ld.so.conf
echo "/usr/lib" >> etc/ld.so.conf
echo "/opt/lib" >> etc/ld.so.conf
/sbin/ldconfig -r $ROOTDIR

# lib
mkdir -p lib/firmware
# the hack to make /opt/lib work with uclibc 0.9.19 ld.so
# which is ignoring ld.so.conf completely
mkdir -p usr/X11R6
ln -sf ../../opt/lib usr/X11R6/lib

#!/bin/sh
#
# Stop dongle drivers and replace with new ones
#
# Copyright 2004, Broadcom Corporation
# All Rights Reserved.                
#                                     
# This is UNPUBLISHED PROPRIETARY SOURCE CODE of Broadcom Corporation;   
# the contents of this file may not be disclosed to third parties, copied
# or duplicated in any form, in whole or in part, without the prior      
# written permission of Broadcom Corporation.                            
#
# $Id$
#

# get old drivers out of the way
/sbin/rmmod usbdev-rndis
/sbin/rmmod wl

# new drivers are in /tmp
cd /tmp

# rename so subsequent rmmod <name> just works
/bin/gunzip -c 1.gz > usbdev-rndis.o
/bin/gunzip -c 2.gz > wl.o
/sbin/insmod ./wl.o
/sbin/insmod ./usbdev-rndis.o slave=eth0
/bin/rm 0.sh 1.gz 2.gz usbdev-rndis.o wl.o

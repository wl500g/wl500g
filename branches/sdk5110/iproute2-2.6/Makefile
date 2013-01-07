#
#
# Copyright (C) 2011 by wl500g.googlecode.com project
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

IPROUTE2=iproute2-2.6.25

define patches_list
    $(shell ls -1 $(1)/[0-9][0-9][0-9]-*.patch 2>/dev/null)
endef

iproute2_Patches := $(call patches_list,.)

$(TOP)/iproute2: $(IPROUTE2).tar.bz2
	@rm -rf $(TOP)/$@
	tar -xjf $^ -C $(TOP)
	$(PATCHER) -Z $(TOP)/$(IPROUTE2) $(iproute2_Patches)
	mv $(TOP)/$(IPROUTE2) $@

iproute2: $(TOP)/iproute2
	@true

.PHONY: iproute2

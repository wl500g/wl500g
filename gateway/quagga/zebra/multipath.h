/*
 * multipath policy names.
 *
 * This file is part of Quagga routing suite.
 *
 * Quagga is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 *
 * Quagga is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Zebra; see the file COPYING.  If not, write to the Free
 * Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#ifndef ZEBRA_MULTIPATH_H
#define ZEBRA_MULTIPATH_H
#include <zebra.h>

#ifdef HAVE_LINUX_IP_MP_ALG_H

static char *mp_alg_names[IP_MP_ALG_MAX+1] = {
                        [IP_MP_ALG_NONE] = "none",
                        [IP_MP_ALG_RR] = "rr",
                        [IP_MP_ALG_DRR] = "drr",
                        [IP_MP_ALG_RANDOM] = "random",
                        [IP_MP_ALG_WRANDOM] = "wrandom"
			};
#endif
#endif


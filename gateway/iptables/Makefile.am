# -*- Makefile -*-

ACLOCAL_AMFLAGS  = -I m4
AUTOMAKE_OPTIONS = foreign subdir-objects

AM_CFLAGS        = ${regular_CFLAGS} -I${top_builddir}/include -I${top_srcdir}/include ${kinclude_CFLAGS}
SUBDIRS          = extensions
if ENABLE_DEVEL
SUBDIRS         += include
endif
if ENABLE_LIBIPQ
SUBDIRS         += libipq
endif
lib_LTLIBRARIES =

# libiptc
lib_LTLIBRARIES           += libiptc/libiptc.la
libiptc_libiptc_la_SOURCES = libiptc/libip4tc.c libiptc/libip6tc.c
libiptc_libiptc_la_LDFLAGS = -version-info 0:0:0

lib_LTLIBRARIES      += libxtables.la
libxtables_la_SOURCES = xtables.c
libxtables_la_LDFLAGS = -version-info ${libxtables_vcurrent}:0:${libxtables_vage}
libxtables_la_LIBADD  = -ldl

# iptables, dynamic
iptables_SOURCES          = iptables-standalone.c iptables.c
iptables_LDFLAGS          = -rdynamic
iptables_LDADD            = libiptc/libiptc.la extensions/libext4.a libxtables.la -lm

iptables_xml_LDADD        = libxtables.la
iptables_multi_SOURCES    = iptables-multi.c iptables-save.c \
                            iptables-restore.c iptables-xml.c \
                            iptables-standalone.c iptables.c
iptables_multi_CFLAGS     = ${AM_CFLAGS} -DIPTABLES_MULTI
iptables_multi_LDFLAGS    = ${iptables_LDFLAGS}
iptables_multi_LDADD      = ${iptables_LDADD}

iptables_restore_SOURCES  = iptables-restore.c iptables.c
iptables_restore_LDFLAGS  = ${iptables_LDFLAGS}
iptables_restore_LDADD    = ${iptables_LDADD}

iptables_save_SOURCES     = iptables-save.c iptables.c
iptables_save_LDFLAGS     = ${iptables_LDFLAGS}
iptables_save_LDADD       = ${iptables_LDADD}

# iptables-multi, semi-static
iptables_static_SOURCES   = ${iptables_multi_SOURCES} xtables.c
iptables_static_CFLAGS    = ${iptables_multi_CFLAGS} -DNO_SHARED_LIBS=1
iptables_static_LDADD     = libiptc/libiptc.la extensions/libext4.a -lm

iptables_xml_SOURCES      = iptables-xml.c

# ip6tables, dynamic
ip6tables_SOURCES         = ip6tables-standalone.c ip6tables.c
ip6tables_LDFLAGS         = -rdynamic
ip6tables_LDADD           = libiptc/libiptc.la extensions/libext6.a libxtables.la -lm

ip6tables_multi_SOURCES   = ip6tables-multi.c ip6tables-save.c \
                            ip6tables-restore.c ip6tables-standalone.c \
                            ip6tables.c
ip6tables_multi_CFLAGS    = ${AM_CFLAGS} -DIPTABLES_MULTI
ip6tables_multi_LDFLAGS   = ${ip6tables_LDFLAGS}
ip6tables_multi_LDADD     = ${ip6tables_LDADD}

ip6tables_restore_SOURCES = ip6tables-restore.c ip6tables.c
ip6tables_restore_LDFLAGS = ${ip6tables_LDFLAGS}
ip6tables_restore_LDADD   = ${ip6tables_LDADD}

ip6tables_save_SOURCES    = ip6tables-save.c ip6tables.c
ip6tables_save_LDFLAGS    = ${ip6tables_LDFLAGS}
ip6tables_save_LDADD      = ${ip6tables_LDADD}

# iptables-multi, semi-static
ip6tables_static_SOURCES    = ${ip6tables_multi_SOURCES} xtables.c
ip6tables_static_CFLAGS     = ${ip6tables_multi_CFLAGS} -DNO_SHARED_LIBS=1
ip6tables_static_LDADD      = libiptc/libiptc.la extensions/libext6.a -lm

bin_PROGRAMS     = iptables-xml
sbin_PROGRAMS    =
noinst_PROGRAMS  =
man_MANS         = iptables.8 iptables-restore.8 iptables-save.8 \
                   iptables-xml.8 ip6tables.8 ip6tables-restore.8 \
                   ip6tables-save.8
CLEANFILES       = iptables.8 ip6tables.8

if ENABLE_STATIC
if ENABLE_IPV4
sbin_PROGRAMS += iptables-static
endif
if ENABLE_IPV6
sbin_PROGRAMS += ip6tables-static
endif
endif
if ENABLE_SHARED
if ENABLE_IPV4
sbin_PROGRAMS += iptables iptables-multi iptables-restore iptables-save
endif
if ENABLE_IPV6
sbin_PROGRAMS += ip6tables ip6tables-multi ip6tables-restore ip6tables-save
endif
endif

iptables.8: ${srcdir}/iptables.8.in extensions/matches4.man extensions/targets4.man
	${AM_VERBOSE_GEN} sed -e 's/@PACKAGE_AND_VERSION@/${PACKAGE} ${PACKAGE_VERSION}/g' -e '/@MATCH@/ r extensions/matches4.man' -e '/@TARGET@/ r extensions/targets4.man' $< >$@;

ip6tables.8: ${srcdir}/ip6tables.8.in extensions/matches6.man extensions/targets6.man
	${AM_VERBOSE_GEN} sed -e 's/@PACKAGE_AND_VERSION@/${PACKAGE} ${PACKAGE_VERSION}/g' -e '/@MATCH@/ r extensions/matches6.man' -e '/@TARGET@/ r extensions/targets6.man' $< >$@;

pkgconfig_DATA = libiptc.pc xtables.pc

.PHONY: tarball
tarball:
	rm -Rf /tmp/${PACKAGE_TARNAME}-${PACKAGE_VERSION};
	pushd ${top_srcdir} && git archive --prefix=${PACKAGE_TARNAME}-${PACKAGE_VERSION}/ HEAD | tar -C /tmp -x && popd;
	pushd /tmp/${PACKAGE_TARNAME}-${PACKAGE_VERSION} && ./autogen.sh && popd;
	tar -C /tmp -cjf ${PACKAGE_TARNAME}-${PACKAGE_VERSION}.tar.bz2 --owner=root --group=root ${PACKAGE_TARNAME}-${PACKAGE_VERSION}/;
	rm -Rf /tmp/${PACKAGE_TARNAME}-${PACKAGE_VERSION};

config.status: extensions/GNUmakefile.in \
	include/xtables.h.in include/iptables/internal.h.in

# Using if..fi avoids an ugly "error (ignored)" message :)
install-exec-hook:
	-if test -z "${DESTDIR}"; then /sbin/ldconfig; fi;

# Build librfc822

AM_YFLAGS = -d
AM_LFLAGS = -i

noinst_LIBRARIES    = librfc822.a
noinst_HEADERS      = rfc822.h
librfc822_a_SOURCES = address.y address_scan.l address_sep.c decomment.c

noinst_PROGRAMS   = test
test_SOURCES      = test.c
test_CPPFLAGS     = -I$(srcdir)/../libgnu
test_LDADD        = librfc822.a ../libgnu/libgnu.a ../libmpools/libmpools.a ../liblists/liblists.a

EXTRA_DIST        = testaddresslines parse_address.c

DISTCLEANFILES    = address.c address_scan.c
MAINTAINERCLEANFILES = Makefile.in

$(builddir)/address_scan.o : $(builddir)/address.h

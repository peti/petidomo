#
# Build the libconfigfile library.
#
# $Header$
#

CC 		= gcc
AR 		= ar
RANLIB		= ranlib

CFLAGS		= -Wall -pedantic -O3
CPPFLAGS	=
LDFLAGS		=

.SUFFIXES:
.SUFFIXES:	.c .o .a

.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

libconfigfile.a:	config.o
	$(AR) cr libconfigfile.a config.o
	$(RANLIB) libconfigfile.a

realclean distclean clean::
	rm -f libconfigfile.a config.o

# Dependencies

config.o: ../liblists/lists.h ../libtext/text.h configfile.h

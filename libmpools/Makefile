#
# Build the libmpools library.
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
.SUFFIXES:	.c .o

.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

libmpools.a:	mpools.o
	$(AR) cr libmpools.a mpools.o
	$(RANLIB) libmpools.a

realclean distclean clean::
	rm -f libmpools.a mpools.o

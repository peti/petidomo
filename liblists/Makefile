#
# Build the liblists library.
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

liblists.a:	lists.o
	$(AR) cr liblists.a lists.o
	$(RANLIB) liblists.a

realclean distclean clean::
	rm -f liblists.a lists.o

#
# Build the libargv library.
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

libargv.a:	argv.o
	$(AR) cr libargv.a argv.o
	$(RANLIB) libargv.a

test:		test.o libargv.a
	$(CC) $(LDFLAGS) test.o libargv.a -o test

realclean distclean clean::
	rm -f libargv.a argv.o test.o test

# Dependencies

argv.o: argv.h argv_loc.h
test.o: argv.h

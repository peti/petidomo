#
# Build the rfc822 Library
#
# $Header$
#

CC		= gcc
AR		= ar
RANLIB		= ranlib

CFLAGS		= -Wall -pedantic -O3
CPPFLAGS	=
LDFLAGS		=

.SUFFIXES:
.SUFFIXES:	.c .o

.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<

OBJS	= address.o address_scan.o decomment.o address_sep.o


librfc822.a:	 $(OBJS)
	$(AR) cr librfc822.a $(OBJS)
	$(RANLIB) librfc822.a

address.c address_scan.h:	address.y rfc822.h parse_address.c
	$(YACC) -d -p "rfc822_" address.y
	mv y.tab.c address.c
	mv y.tab.h address_scan.h

address_scan.c:		address_scan.l rfc822.h
	$(LEX) -Prfc822_ address_scan.l
	mv lex.rfc822_.c $@

realclean distclean clean::
	rm -f librfc822.a test test.o
	rm -f address.c address_scan.c address_scan.h
	rm -rf $(OBJS)

# Dependencies

address.o:		address.c address_scan.h address_scan.c parse_address.c
address_scan.c:		address_scan.l
decomment.c:		rfc822.h
address_sep.c:		rfc822.h

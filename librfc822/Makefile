#
# RFC Parse Library
#
# $Header$
#

# Make Rules:
# ===========
#
.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<


# Compiler flags:
# ===============
#
CFLAGS	= -Wall
CPPFLAGS=

# Linker flags:
# =============
#
LDFLAGS	=
LIBS	= ../libmpools/libmpools.a ../liblists/liblists.a


#
# Labels:
# =======
#
SRCS	= address.c address_scan.c decomment.c address_sep.c
OBJS	= $(SRCS:.c=.o)


#
# Targets
#
.PHONY: all man clean realclean depend

all:	librfc822.a

man:	rfc822_decomment.3 rfc822_parse_address.3 rfc822_address_sep.3

test:	test.o librfc822.a rfc822.h $(LIBS)
	$(CC) test.o librfc822.a -o $@ $(LDFLAGS) $(LIBS)

clean:
	rm -f librfc822.a test *.o *.3 *.core *.bak

realclean:	clean
	rm -f address.c address_scan.c address_scan.h

depend:
	makedepend -Y /usr/include $(SRCS)
	@rm -f Makefile.bak


#
# Actions:
#=========
#
librfc822.a:	$(OBJS)
	$(AR) cr $@ $(OBJS)
	$(RANLIB) $@

address.c address_scan.h:	address.y rfc822.h parse_address.c
	$(YACC) -d -p "rfc822_" address.y
	mv y.tab.c address.c
	mv y.tab.h address_scan.h

address_scan.c:		address_scan.l rfc822.h
	$(LEX) -Prfc822_ address_scan.l
	mv lex.rfc822_.c $@

rfc822_decomment.3:	decomment.c
	c2man -irfc822.h decomment.c

rfc822_parse_address.3:	parse_address.c
	c2man -irfc822.h parse_address.c

rfc822_address_sep.3:	address_sep.c
	c2man -irfc822.h address_sep.c

../libmpools/libmpools.a:
	(cd ../libmpools;$(MAKE))

../liblists/liblists.a:
	(cd ../liblists;$(MAKE))

#
# Dependencies
#

address.o:		address.c address_scan.h address_scan.c parse_address.c
address_scan.c:		address_scan.l
decomment.c:		rfc822.h
address_sep.c:		rfc822.h

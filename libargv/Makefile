#
# libargv Makefile
#
# $Header$
#

# Make Rules:
# ===========
#
.c.o:
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $<


# Compiler Flags:
# ===============
#
CFLAGS  = -Wall -ansi -pedantic
CPPFLAGS=


#
# Labels:
# =======
#
SRCS 	= argv.c
OBJS	= $(SRCS:.c=.o)
MANFILES= $(SRCS:.c=.3)


#
# Targets
#
.PHONY: all clean realclean distclean man depend

all:	libargv.a

test:		test.o libargv.a
	$(CC) test.o libargv.a -o test

clean:
	rm -f test libargv.a *.o *.3 *.core

realclean:	clean

distclean:	realclean

man:

depend:
	makedepend -Y /usr/include $(SRCS)
	@rm -f Makefile.bak


#
# Actions
#
libargv.a:	$(OBJS)
	rm -f $@
	$(AR) cr $@ $(OBJS)
	$(RANLIB) $@


#
# Dependencies
#

argv.o: _argv.h argv.h

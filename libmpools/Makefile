#
# libmpools Makefile
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
CFLAGS  = -Wall
CPPFLAGS=


#
# Labels:
# =======
#
SRCS 	= mpools.c
OBJS	= $(SRCS:.c=.o)
MANFILES= $(SRCS:.c=.3)


#
# Targets
#
.PHONY: all man clean realclean distclean depend

all:	libmpools.a

man:	mp_malloc.3

clean:
	rm -f libmpools.a *.o *.3 *.core

realclean:	clean
	rm -rf man3

distclean:	realclean

depend:
	makedepend -Y /usr/include $(SRCS)
	@rm -f Makefile.bak

mp_malloc.3:	mpools.c
	c2man -impools.h -g mpools.c;


#
# Actions
#
libmpools.a:	$(OBJS)
	rm -f $@
	$(AR) cr $@ $(OBJS)
	$(RANLIB) $@


#
# Dependencies
#

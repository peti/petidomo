#
# liblists Makefile
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
SRCS 	= lists.c
OBJS	= $(SRCS:.c=.o)
MANFILES= $(SRCS:.c=.3)


#
# Targets
#
.PHONY: all man clean realclean distclean depend

all:	liblists.a

man:	InitList.3

clean:
	rm -f liblists.a *.o *.3 *.core

realclean:	clean
	rm -rf man3

distclean:	realclean

depend:
	makedepend -Y /usr/include $(SRCS)
	@rm -f Makefile.bak

InitList.3:	lists.c
	c2man -ilists.h -g lists.c


#
# Actions
#
liblists.a:	$(OBJS)
	rm -f $@
	$(AR) cr $@ $(OBJS)
	$(RANLIB) $@


#
# Dependencies
#
# DO NOT DELETE

lists.o: lists.h

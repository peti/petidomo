#
# libconfigfile Makefile
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
SRCS 	= config.c
OBJS	= $(SRCS:.c=.o)


#
# Targets
#
.PHONY: all man clean depend

all:	libconfigfile.a

man:

clean:
	rm -f libconfigfile.a *.o *.3 *.core

depend:
	makedepend -Y /usr/include $(SRCS)
	@rm -f Makefile.bak

#
# Actions
#
libconfigfile.a:	$(OBJS)
	rm -f $@
	$(AR) cr $@ $(OBJS)
	$(RANLIB) $@


#
# Dependencies
#

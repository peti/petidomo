#
# libargv
#
# $Id$
#

./libargv.a:	./argv.o
	$(AR) cr $@ ./argv.o
	$(RANLIB) $@

./argv.o:	./argv.c ./_argv.h ./argv.h

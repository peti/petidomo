#
# Petidomo Makefile
#
# $Header$
#

include ../../include/petidomo.mk

SRCS	= archive.c authen.c config.c exit.c filter.c handleacl.c help.c \
	  hermes.c index.c io.c listserv.c mailer.c main.c members.c \
	  parsearray.c password.c rfcparse.c subscribe.c tool.c unregsig.c \
	  unsubscribe.c acl.c argvSetDebugLevel.c
OBJS	= ${SRCS:.c=.o}

#
# Targets
#
.PHONY: all clean distclean realclean depend

all:	hermes listserv

clean:
	rm -f petidomo listserv hermes *.o *.core *.bak

distclean:	clean

realclean:	distclean
	rm -f acl_scan.c acl_scan.h acl.c

depend:
	makedepend -Y /usr/include ${SRCS}
	@rm -f Makefile.bak

petidomo:	${OBJS}
	${CC} ${OBJS} -o petidomo ${LDFLAGS} -lconfigfile -largv -ldebug -ltext \
		-lrfc822 -lmpools -llists -lcompat ${LIBS}

listserv:	petidomo
	rm -f listserv
	ln petidomo listserv

hermes:		petidomo
	rm -f hermes
	ln petidomo hermes

acl.c:		acl.y
	$(YACC) -d -p acl $<
	mv y.tab.c acl.c
	mv y.tab.h acl_scan.h

acl_scan.c:	acl_scan.l acl.c
	$(LEX) -i -Pacl $<
	mv lex.acl.c acl_scan.c

unregsig.o:	unregsig.c
	$(CC) $(CPPFLAGS) $(UCBINCLUDE) $(CFLAGS) -c unregsig.c

#
# Dependencies
#

acl.o: acl_scan.c acl_scan.h
acl_scan.o: acl_scan.h
main.o: version.h
unregsig.o: version.h
archive.o: rfcparse.h
mailer.o: rfcparse.h

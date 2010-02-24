#
# build petidomo
#

ACLOCAL_AMFLAGS = -I build-aux
AM_YFLAGS = -d

SUBDIRS = libgnu libargv libtext liblists librfc822 libmpools libconfigfile doc

sbin_PROGRAMS = petidomo

petidomo_SOURCES =                                              \
  acl-parser.y acl-scanner.l approve.c config-files.c           \
  handleacl.c index.c parsearray.c queue-posting.c              \
  tool.c archive.c filter.c help.c io.c main.c                  \
  password.c rfcparse.c unsubscribe.c address-db.c              \
  authen.c generate-cookie.c hermes.c listserv.c                \
  members.c mailer.c queue-command.c subscribe.c

petidomo_CPPFLAGS =                                             \
  -I${srcdir}/libgnu                                            \
  -DMTA=\"${mta}\"                                              \
  -DSYSCONFDIR=\"${sysconfdir}\"                                \
  -DLOCALSTATEDIR=\"${localstatedir}\"                          \
  -DDATADIR=\"${datadir}\"

petidomo_LDADD =                                                \
  libconfigfile/libconfigfile.a libtext/libtext.a               \
  liblists/liblists.a librfc822/librfc822.a                     \
  libmpools/libmpools.a libargv/libargv.a libgnu/libgnu.a

noinst_HEADERS  = petidomo.h
BUILT_SOURCES   = acl-parser.c acl-scanner.c acl-parser.h

EXTRA_DIST      = build-aux/gnulib-cache.m4

CLEANFILES      = petidomo-*.tar.gz
DISTCLEANFILES  = $(BUILT_SOURCES) $(EXTRA_DIST)
MAINTAINERCLEANFILES =                                          \
  Makefile.in aclocal.m4 compile config.guess config.sub        \
  configure liblists/Makefile.in     \
  libmpools/Makefile.in libargv/Makefile.in librfc822/Makefile.in

maintainer-clean-local:
	-rm -rf build-aux

# install:	all
# 	$(SHTOOL) mkdir -f -p -m 755 $(DESTDIR)$(bindir)
# 	$(SHTOOL) mkdir -f -p -m 755 $(DESTDIR)$(sysconfdir)
# 	$(SHTOOL) mkdir -f -p -m 755 $(DESTDIR)$(localstatedir)/lists
# 	$(SHTOOL) mkdir -f -p -m 755 $(DESTDIR)$(localstatedir)/ack-queue
# 	$(SHTOOL) mkdir -f -p -m 755 $(DESTDIR)$(datadir)
# 	$(SHTOOL) mkdir -f -p -m 755 $(DESTDIR)$(libexecdir)
# 	$(SHTOOL) mkdir -f -p -m 755 $(DESTDIR)$(mandir)/man1
# 	$(SHTOOL) install -c -s -m 555 petidomo $(DESTDIR)$(bindir)
# 	$(SHTOOL) install -c -m 755 petidomo-approve $(DESTDIR)$(bindir)
# 	$(SHTOOL) install -c -m 755 petidomo-kickout $(DESTDIR)$(bindir)
# 	$(SHTOOL) install -c -m 644 petidomo.1 $(DESTDIR)$(mandir)/man1/petidomo.1
# 	$(SHTOOL) install -c -m 644 config/petidomo.conf $(DESTDIR)$(sysconfdir)/petidomo.conf-sample
# 	$(SHTOOL) install -c -m 644 config/petidomo.acl $(DESTDIR)$(sysconfdir)/petidomo.acl-sample
# 	$(SHTOOL) install -c -m 444 config/help $(DESTDIR)$(datadir)
# 	if [ ! -f $(DESTDIR)$(localstatedir)/index ] ; then $(SHTOOL) install -c -m 444 config/index $(DESTDIR)$(localstatedir); fi
# 	$(SHTOOL) install -c -m 555 filter/insert-name-in-subject.sh $(DESTDIR)$(libexecdir)
# 	$(SHTOOL) install -c -m 555 filter/pgp-decrypt.sh $(DESTDIR)$(libexecdir)
# 	$(SHTOOL) install -c -m 555 filter/pgp-encrypt.sh $(DESTDIR)$(libexecdir)
# 	$(SHTOOL) install -c -m 555 filter/rfc2369.sh $(DESTDIR)$(libexecdir)
#
# install-testlist:
# 	$(SHTOOL) mkdir -f -p -m 755 $(DESTDIR)$(localstatedir)/lists/testlist
# 	$(SHTOOL) install -c -m 644 config/list-config $(DESTDIR)$(localstatedir)/lists/testlist/config
# 	$(SHTOOL) install -c -m 644 config/list-acl $(DESTDIR)$(localstatedir)/lists/testlist/acl
# 	@rm -f $(DESTDIR)$(localstatedir)/lists/testlist/list
# 	$(SHTOOL) install -c -m 644 /dev/null $(DESTDIR)$(localstatedir)/lists/testlist/list
#
# #   increase or update version information
# new-version:
# 	@V="$(VERSION)"; OPT="-e"; [ ".$$V" != . ] && OPT="-s$$V"; \
# 	$(SHTOOL) version -lc -n 'Petidomo' -p petidomo_ $$OPT version.c
# update-version:
# 	@OPT="-s`$(SHTOOL) version -lc -dshort version.c`"; \
# 	$(SHTOOL) version -lc -n 'Petidomo' -p petidomo_ $$OPT version.c

dist-hook:
	echo $(VERSION) > $(distdir)/.tarball-version
	if test -d .git; then						\
	  $(top_srcdir)/build-aux/gitlog-to-changelog			\
	    >$(distdir)/cl-t $(top_srcdir);				\
	  rm -f $(distdir)/ChangeLog;					\
	  mv $(distdir)/cl-t $(distdir)/ChangeLog;			\
	fi

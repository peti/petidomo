#
# build petidomo
#

ACLOCAL_AMFLAGS = -I build-aux
AM_YFLAGS = -d

SUBDIRS = libgnu libargv libtext liblists libmpools librfc822 libconfigfile doc

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

EXTRA_DIST      = build-aux/gnulib-cache.m4 petidomo-approve.pl \
                  petidomo-kickout.pl filter/rfc2369.sh         \
                  filter/insert-name-in-subject.sh              \
                  filter/pgp-decrypt.sh filter/pgp-encrypt.sh   \
                  config/petidomo.conf config/petidomo.acl      \
                  config/help config/index config/list-config   \
                  config/list-acl

DISTCLEANFILES  = $(BUILT_SOURCES)
MAINTAINERCLEANFILES =                                          \
  Makefile.in aclocal.m4 compile config.guess config.sub        \
  configure ChangeLog GNUmakefile config.h.in maint.mk INSTALL

maintainer-clean-local:
	-rm -rf build-aux libgnu

distclean-local:
	-rm -rf autom4te.cache

install-exec-local:
	test -d "$(DESTDIR)$(sbindir)" || $(MKDIR_P) "$(DESTDIR)$(sbindir)"
	test -d "$(DESTDIR)$(libexecdir)" || $(MKDIR_P) "$(DESTDIR)$(libexecdir)"
	$(INSTALL_PROGRAM) ${srcdir}/petidomo-approve.pl "$(DESTDIR)$(sbindir)/petidomo-approve"
	$(INSTALL_PROGRAM) ${srcdir}/petidomo-kickout.pl "$(DESTDIR)$(sbindir)/petidomo-kickout"
	$(INSTALL_PROGRAM) ${srcdir}/filter/insert-name-in-subject.sh "$(DESTDIR)$(libexecdir)"
	$(INSTALL_PROGRAM) ${srcdir}/filter/pgp-decrypt.sh "$(DESTDIR)$(libexecdir)"
	$(INSTALL_PROGRAM) ${srcdir}/filter/pgp-encrypt.sh "$(DESTDIR)$(libexecdir)"
	$(INSTALL_PROGRAM) ${srcdir}/filter/rfc2369.sh "$(DESTDIR)$(libexecdir)"

install-data-local:
	test -d "$(DESTDIR)$(sysconfdir)" || $(MKDIR_P) "$(DESTDIR)$(sysconfdir)"
	test -d "$(DESTDIR)$(localstatedir)/lists" || $(MKDIR_P) "$(DESTDIR)$(localstatedir)/lists"
	test -d "$(DESTDIR)$(localstatedir)/ack-queue" || $(MKDIR_P) "$(DESTDIR)$(localstatedir)/ack-queue"
	test -d "$(DESTDIR)$(datadir)" || $(MKDIR_P) "$(DESTDIR)$(datadir)"
	$(INSTALL_DATA) ${srcdir}/config/petidomo.conf "$(DESTDIR)$(sysconfdir)/petidomo.conf-sample"
	$(INSTALL_DATA) ${srcdir}/config/petidomo.acl "$(DESTDIR)$(sysconfdir)/petidomo.acl-sample"
	$(INSTALL_DATA) ${srcdir}/config/help "$(DESTDIR)$(datadir)"
	if [ ! -f "$(DESTDIR)$(localstatedir)/index" ] ; then $(INSTALL_DATA) ${srcdir}/config/index "$(DESTDIR)$(localstatedir)"; fi

install-testlist:
	test -d "$(DESTDIR)$(localstatedir)/lists/testlist" || $(MKDIR_P) "$(DESTDIR)$(localstatedir)/lists/testlist"
	$(INSTALL_DATA) ${srcdir}/config/list-config "$(DESTDIR)$(localstatedir)/lists/testlist/config"
	$(INSTALL_DATA) ${srcdir}/config/list-acl "$(DESTDIR)$(localstatedir)/lists/testlist/acl"
	@rm -f "$(DESTDIR)$(localstatedir)/lists/testlist/list"
	$(INSTALL_DATA) /dev/null "$(DESTDIR)$(localstatedir)/lists/testlist/list"

uninstall-local:
	rm -f "$(DESTDIR)$(sbindir)/petidomo-kickout"
	rm -f "$(DESTDIR)$(sbindir)/petidomo-approve"
	rm -f "$(DESTDIR)$(sysconfdir)/petidomo.acl-sample"
	rm -f "$(DESTDIR)$(sysconfdir)/petidomo.conf-sample"
	rm -f "$(DESTDIR)$(libexecdir)/rfc2369.sh"
	rm -f "$(DESTDIR)$(libexecdir)/insert-name-in-subject.sh"
	rm -f "$(DESTDIR)$(libexecdir)/pgp-encrypt.sh"
	rm -f "$(DESTDIR)$(libexecdir)/pgp-decrypt.sh"
	rm -f "$(DESTDIR)$(localstatedir)/index"
	rm -f "$(DESTDIR)$(datadir)/help"

dist-hook:
	echo $(VERSION) > $(distdir)/.tarball-version
	if test -d .git; then						\
	  $(top_srcdir)/build-aux/gitlog-to-changelog			\
	    >$(distdir)/cl-t $(top_srcdir);				\
	  rm -f $(distdir)/ChangeLog;					\
	  mv $(distdir)/cl-t $(distdir)/ChangeLog;			\
	fi

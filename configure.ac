dnl configure.ac -- Process this file with autoconf to produce a configure script.

AC_INIT([petidomo], m4_esyscmd([build-aux/git-version-gen .tarball-version]), [simons@cryp.to], [petidomo], [http://petidomo.sourceforge.net/])
AC_CONFIG_SRCDIR([petidomo.h])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([1.11 -Wall gnu])
AC_CONFIG_MACRO_DIR([build-aux])

AC_COPYRIGHT([dnl
Copyright (C) 1997-2013 Peter Simons <simons@cryp.to>
Copyright (C) 1999-2000 CyberSolutions GmbH, Germany.
Copyright (C) 2000 Cable & Wireless Corp.

Petidomo is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation, either version 3 of the License, or (at your
option) any later version.

Petidomo is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
more details.

You should have received a copy of the GNU General Public License along
with Petidomo. If not, see <http://www.gnu.org/licenses/>.])

AC_MSG_NOTICE([petidomo version $VERSION])

AC_PROG_CC
gl_EARLY
AM_PROG_CC_C_O
AM_PROG_AR
AC_PROG_RANLIB
AM_PROG_LEX
if test "$LEX" != "flex"; then
  AC_MSG_WARN([$LEX might not work; install flex if the build fails.])
fi
AC_PROG_YACC
if test "$YACC" != "bison -y"; then
  AC_MSG_WARN([$YACC might not work; install bison if the build fails.])
fi
gl_INIT

AC_SUBST(mta)
AC_ARG_WITH(mta, [  --with-mta=PATH         location of the mail transport agent (sendmail?)],
    [
    if test "$withval" = "no" -o "$withval" = "yes"; then
        AC_MSG_ERROR([The --with-mta option expects a path as parameter!])
    fi
    AC_MSG_CHECKING(for the location of the mail transport agent)
    mta=$withval
    AC_MSG_RESULT($mta)
    if test ! -f $mta; then
        AC_MSG_WARN(The specified MTA '$mta' does not exist.)
    elif test ! -x $mta; then
        AC_MSG_WARN(The specified MTA '$mta' is not executable.)
    fi
    ],
    [
    AC_PATH_PROG(mta, sendmail, none, $PATH:/usr/lib:/etc/usr/etc)
    if test "$mta" = "none"; then
        AC_MSG_ERROR(Cannot locate your mail transport agent.)
    fi
    ])

dnl Include debug code?

AC_MSG_CHECKING([whether to include debugging capabilities])
AC_ARG_WITH(debug, [  --with-debug            Support debugging? (default: yes)],
    [
    if test "$withval" = "no"; then
       AC_MSG_RESULT(no)
    elif test "$withval" = "yes"; then
       CPPFLAGS="$CPPFLAGS -DDEBUG"
       AC_MSG_RESULT(yes)
    fi
    ],
    [
    CPPFLAGS="$CPPFLAGS -DDEBUG"
    AC_MSG_RESULT(yes)
    ])

dnl other system stuff

AC_SYS_LARGEFILE

dnl Grande Finale

AC_CONFIG_HEADER([config.h])
AC_CONFIG_FILES([
    Makefile
    doc/Makefile
    libgnu/Makefile
    libargv/Makefile
    libconfigfile/Makefile
    liblists/Makefile
    libmpools/Makefile
    librfc822/Makefile
    libtext/Makefile
])
AC_OUTPUT

#! /usr/bin/env bash

set -eu

if [ -x "gnulib/gnulib-tool" ]; then
  gnulibtool=gnulib/gnulib-tool
else
  gnulibtool=gnulib-tool
fi

gnulib_modules=( git-version-gen gitlog-to-changelog gnupload progname
                 maintainer-makefile announce-gen stddef crypto/md5 )

$gnulibtool --m4-base build-aux --source-base libgnu --import "${gnulib_modules[@]}"

sed -i -e 's/^sc_bindtextdomain/disabled_sc_bindtextdomain/' \
       -e 's/sc_cast_of_argument_to_free/disabled_sc_cast_of_argument_to_free/' \
       -e 's/^sc_prohibit_atoi_atof/disabled_sc_prohibit_atoi_atof/' \
       -e 's/^sc_prohibit_magic_number_exit/disabled_sc_prohibit_magic_number_exit/' \
       -e 's/^sc_prohibit_strcmp/disabled_sc_prohibit_strcmp/' \
       -e 's/^sc_prohibit_strcpy/disabled_sc_prohibit_strcpy/' \
       -e 's/^sc_prohibit_strncpy/disabled_sc_prohibit_strncpy/' \
       -e 's/^sc_prohibit_test_minus_ao/disabled_sc_prohibit_test_minus_ao/' \
    maint.mk

build-aux/gitlog-to-changelog >ChangeLog

autoreconf --install -Wall

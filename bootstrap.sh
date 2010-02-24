#! /bin/sh

set -eu

if [ -x "gnulib/gnulib-tool" ]; then
  gnulibtool=gnulib/gnulib-tool
else
  gnulibtool=gnulib-tool
fi

gnulib_modules=( git-version-gen gitlog-to-changelog gnupload
                 maintainer-makefile announce-gen crypto/md5 )

$gnulibtool --m4-base build-aux --source-base libgnu --import "${gnulib_modules[@]}"

sed -i -e 's/^sc_prohibit_atoi_atof/disabled_sc_prohibit_atoi_atof/' \
   maint.mk

#        -e 's/^sc_prohibit_magic_number_exit/disabled_sc_prohibit_magic_number_exit/' \
#        -e 's/^sc_prohibit_strcmp/disabled_sc_prohibit_strcmp/' \
#        -e 's/^sc_require_config_h/disabled_sc_require_config_h/' \
#        -e 's/^sc_useless_cpp_parens/disabled_sc_useless_cpp_parens/' \


build-aux/gitlog-to-changelog >ChangeLog

autoreconf --install -Wall

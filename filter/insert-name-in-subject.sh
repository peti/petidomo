#! /bin/sh
#
# $Header$
#
# Insert the name specified on the command line to the Subject of
# every posted article. To use this posting filter, set the following
# in the list's config file:
#
# PostingFilter	"~petidomo/bin/InsertNameInSubject.sh listname"
#

sed -e "1,/^$/ {
    /^[Ss][Uu][Bb][Jj][Ee][Cc][Tt]:*/ {
        s/[Rr][Ee]: *\[$1\] [Rr][Ee]:/Re:/
        s/\[$1\] //
        s/^\([Ss][Uu][Bb][Jj][Ee][Cc][Tt]:\)[    ]*/\1 \[$1\] /
    }
}"

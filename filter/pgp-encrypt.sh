#! /bin/sh
#
# Encrypt an incoming PGP for the subscribers of list so and so.
#
# $Header$
#

PDHOME=`csh -c "echo ~petidomo" | sed -e 's#/$##'`

#
# Please customize these things for your system.
#
PGP=/usr/local/bin/pgp
PGPPATH=$PDHOME/.pgp
PASSWORD="DecryptMe"

#
# Check command line syntax.
#
if [ ! $# = 1 ]; then
    echo >&2 "Error: Wrong number of arguments."
    exit 2
else
    LISTNAME=$1
fi

#
# Declare temporary files we'll need.
#
TMPFILE=/tmp/pgp-encrypt.$$
HEADER=$TMPFILE.header
BODY=$TMPFILE.body
NEWBODY=$TMPFILE.newbody
LOGFILE=$TMPFILE.log
trap 'rm -f $TMPFILE $HEADER $BODY $NEWBODY $LOGFILE; exit' 0 2 3 5 10 13 15

#
# Setup the environment where we will pass PGP the password.
#
PGPPASSFD=0
export PGPPATH PGPPASSFD

#
# Save a copy of the mail we receive on standard input.
#
umask 077
tee $TMPFILE | sed -n -e '1,/^$/p' >$HEADER
sed -n -e '/^$/,$p' <$TMPFILE | sed -e '1d' >$BODY

#
# Encrypt the article.
#
if (echo $PASSWORD;cat $BODY) | $PGP -ftesa -@ $PDHOME/lists/$LISTNAME/list >$NEWBODY 2>$LOGFILE; then
    /usr/bin/logger -p mail.info pgp-encrypt[$$]: Encrypted incoming mail successfully.
    cat $HEADER $NEWBODY
    return 0;
else
    /usr/bin/logger -p mail.info pgp-encrypt[$$]: An error occured while encrypting the mail.
    return 2;
fi

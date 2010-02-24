#! /bin/sh
#
# Decrypt an incoming PGP if necessary.
#

PDHOME=`csh -c "echo ~petidomo" | sed -e 's#/$##'`

#
# Please customize these things for your system.
#
PGP=/usr/local/bin/pgp
export PGPPATH=$PDHOME/.pgp
PASSWORD="DecryptMe"

#
# Declare temporary files we'll need.
#
TMPFILE=/tmp/pgp-decrypt.$$
HEADER=$TMPFILE.header
BODY=$TMPFILE.body
LOGFILE=$TMPFILE.log
trap 'rm -f $TMPFILE $HEADER $BODY $LOGFILE; exit' 0 2 3 5 10 13 15

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

#
# Decrypt the incoming article.
#
if (echo $PASSWORD;cat $TMPFILE) | $PGP -f >$BODY 2>$LOGFILE; then
    /usr/bin/logger -p mail.info pgp-decrypt[$$]: Decrypted incoming mail successfully.
    cat $HEADER $BODY
    return 0;
else
    /usr/bin/logger -p mail.info pgp-decrypt[$$]: An error occured while decrypting the mail.
    return 2;
fi

#! /bin/sh
#
# Add headers to a posting as specified in RFC 2369.
#

# Check command line.

if [ ! $# = 2 ]; then
    echo >&1 Usage: $0 listname fqdn
    exit 2;
fi

# Our temporary files.

tempfile=/tmp/rfc2369.$$
header=/tmp/rfc2369.header.$$
body=/tmp/rfc2369.body.$$
trap "rm -f $tempfile $header $body" 0

umask 0177

# Copy the mail into a temporary file.

cat >$tempfile


# Extract header and body.

sed -n -e '1,/^$/p' <$tempfile | sed -e '/^$/d'  >$header
sed -n -e '/^$/,$p' <$tempfile >$body


# Add the appropriate RFC 2369 headers.

cat >>$header <<EOF
List-Owner: <mailto:$1-request@$2> (The Mailing List Owner)
List-Unsubscribe: <mailto:$1-request@$2?body=unsubscribe>
EOF

# Print the result.

cat $header
cat $body

# Bye!

exit 0;

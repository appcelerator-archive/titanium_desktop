#!/bin/bash
echo Uncompressing archive...
export UNPACKDIR=`mktemp -d /tmp/kXXXXXX`
ARCHIVE=`awk '/^__ARCHIVE_BELOW__/ {print NR + 1; exit 0; }' $0`
tail -n+$ARCHIVE $0 | tar xz -C $UNPACKDIR

$UNPACKDIR/APPNAME
rm -rf $UNPACKDIR

exit 0
__ARCHIVE_BELOW__

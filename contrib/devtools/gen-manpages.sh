#!/bin/sh

TOPDIR=${TOPDIR:-$(git rev-parse --show-toplevel)}
SRCDIR=${SRCDIR:-$TOPDIR/src}
MANDIR=${MANDIR:-$TOPDIR/doc/man}

LITECOINZD=${LITECOINZD:-$SRCDIR/litecoinzd}
CLI=${LITECOINZCLI:-$SRCDIR/litecoinz-cli}
LITECOINZTX=${LITECOINZTX:-$SRCDIR/litecoinz-tx}

[ ! -x $LITECOINZD ] && echo "$LITECOINZD not found or not executable." && exit 1

# The autodetected version git tag can screw up manpage output a little bit
ZECVERSTR=$($LITECOINZCLI --version | head -n1 | awk '{ print $NF }')
ZECVER=$(echo $ZECVERSTR | awk -F- '{ OFS="-"; NF--; print $0; }')
ZECCOMMIT=$(echo $ZECVERSTR | awk -F- '{ print $NF }')

# Create a footer file with copyright content.
# This gets autodetected fine for litecoinzd if --version-string is not set,
# but has different outcomes for litecoinz-cli.
echo "[COPYRIGHT]" > footer.h2m
$LITECOINZD --version | sed -n '1!p' >> footer.h2m

for cmd in $LITECOINZD $LITECOINZCLI $LITECOINZTX; do
  cmdname="${cmd##*/}"
  help2man -N --version-string=$ZECVER --include=footer.h2m -o ${MANDIR}/${cmdname}.1 ${cmd}
  sed -i "s/\\\-$ZECCOMMIT//g" ${MANDIR}/${cmdname}.1
done

rm -f footer.h2m

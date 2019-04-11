#!/usr/bin/env bash

export LC_ALL=C
TOPDIR=${TOPDIR:-$(git rev-parse --show-toplevel)}
BUILDDIR=${BUILDDIR:-$TOPDIR}

BINDIR=${BINDIR:-$BUILDDIR/src}
MANDIR=${MANDIR:-$TOPDIR/doc/man}

LITECOINZD=${LITECOINZD:-$BINDIR/litecoinzd}
LITECOINZCLI=${LITECOINZCLI:-$BINDIR/litecoinz-cli}
LITECOINZTX=${LITECOINZTX:-$BINDIR/litecoinz-tx}
LITECOINZQT=${LITECOINZQT:-$BINDIR/qt/litecoinz-qt}

[ ! -x $LITECOINZD ] && echo "$LITECOINZD not found or not executable." && exit 1

# The autodetected version git tag can screw up manpage output a little bit
LTZVER=($($LITECOINZCLI --version | head -n1 | awk -F'[ -]' '{ print $6, $7 }'))

# Create a footer file with copyright content.
# This gets autodetected fine for litecoinzd if --version-string is not set,
# but has different outcomes for litecoinz-qt and litecoinz-cli.
echo "[COPYRIGHT]" > footer.h2m
$LITECOINZD --version | sed -n '1!p' >> footer.h2m

for cmd in $LITECOINZD $LITECOINZCLI $LITECOINZTX $LITECOINZQT; do
  cmdname="${cmd##*/}"
  help2man -N --version-string=${LTZVER[0]} --include=footer.h2m -o ${MANDIR}/${cmdname}.1 ${cmd}
  sed -i "s/\\\-${LTZVER[1]}//g" ${MANDIR}/${cmdname}.1
done

rm -f footer.h2m

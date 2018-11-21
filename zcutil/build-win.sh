#!/usr/bin/env bash

set -eu -o pipefail

HOST=x86_64-w64-mingw32
BUILD=x86_64-w64-mingw32

function cmd_pref() {
    if type -p "$2" > /dev/null; then
        eval "$1=$2"
    else
        eval "$1=$3"
    fi
}

# If a g-prefixed version of the command exists, use it preferentially.
function gprefix() {
    cmd_pref "$1" "g$2" "$2"
}

gprefix READLINK readlink
cd "$(dirname "$("$READLINK" -f "$0")")/.."

# Allow user overrides to $MAKE. Typical usage for users who need it:
#   MAKE=gmake ./zcutil/build.sh -j$(nproc)
if [[ -z "${MAKE-}" ]]; then
    MAKE=make
fi

if [ "x$*" = 'x--help' ]
then
    cat <<EOF
Usage:

$0 --help
  Show this help message and exit.

$0 [ --disable-mining ] [ MAKEARGS... ]
  Build LitecoinZ and most of its transitive dependencies from
  source. MAKEARGS are applied to both dependencies and LitecoinZ itself.

  If --disable-mining is passed, LitecoinZ is configured to not build any mining
  code. It must be passed after the test arguments, if present.
EOF
    exit 0
fi

set -x

# If --disable-mining is the next argument, disable mining code:
MINING_ARG=''
if [ "x${1:-}" = 'x--disable-mining' ]
then
    MINING_ARG='--enable-mining=no'
    shift
fi

PREFIX="$(pwd)/depends/$BUILD/"

HOST="$HOST" "$MAKE" "$@" -C ./depends/
./autogen.sh
./configure --prefix="${PREFIX}" "$MINING_ARG" --enable-hardening --with-libs=no --enable-tests=no --disable-gui-tests --enable-static --enable-shared=no CXXFLAGS='-g'
HOST="$HOST" "$MAKE" "$@"

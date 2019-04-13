UNIX BUILD NOTES
====================
Some notes on how to build LitecoinZ Core in Unix.

Note
---------------------
Always use absolute paths to configure and compile LitecoinZ Core and the dependencies,
for example, when specifying the path of the dependency:

	../dist/configure --enable-cxx --disable-shared --with-pic --prefix=$BDB_PREFIX

Here BDB_PREFIX must be an absolute path - it is defined using $(pwd) which ensures
the usage of the absolute path.

To Build
---------------------

```bash
HOST=$($PWD/depends/config.guess)
HOST="$HOST" make -C $PWD/depends
./autogen.sh
CONFIG_SITE="$PWD/depends/$HOST/share/config.site" ./configure
make
make install # optional
```

This will build litecoinz-qt as well. To build without GUI pass `--without-gui`.

Memory Requirements
--------------------

C++ compilers are memory-hungry. It is recommended to have at least 1.5 GB of
memory available when compiling LitecoinZ Core. On systems with less, gcc can be
tuned to conserve memory with additional CXXFLAGS:


    ./configure CXXFLAGS="--param ggc-min-expand=1 --param ggc-min-heapsize=32768"


## Linux Distribution Specific Instructions

### Ubuntu & Debian

#### Dependency Build Instructions

Build requirements:

    sudo apt-get install build-essential libtool autotools-dev automake pkg-config bsdmainutils python3 libc6-dev m4 g++-multilib autoconf ncurses-dev unzip git wget curl zlib1g-dev

See the section "Disable-wallet mode" to build LitecoinZ Core without wallet.

### Fedora

#### Dependency Build Instructions

Build requirements:

    sudo dnf install gcc-c++ libtool make autoconf automake python3 pkgconfig wget curl gcc gcc-c++ patch

Notes
-----
The release is built with GCC and then "strip litecoinzd" to strip the debug
symbols, which reduces the executable size by about 90%.

Security
--------
To help make your LitecoinZ Core installation more secure by making certain attacks impossible to
exploit even if a vulnerability is found, binaries are hardened by default.
This can be disabled with:

Hardening Flags:

	./configure --enable-hardening
	./configure --disable-hardening


Hardening enables the following features:

* Position Independent Executable
    Build position independent code to take advantage of Address Space Layout Randomization
    offered by some kernels. Attackers who can cause execution of code at an arbitrary memory
    location are thwarted if they don't know where anything useful is located.
    The stack and heap are randomly located by default, but this allows the code section to be
    randomly located as well.

    On an AMD64 processor where a library was not compiled with -fPIC, this will cause an error
    such as: "relocation R_X86_64_32 against `......' can not be used when making a shared object;"

    To test that you have built PIE executable, install scanelf, part of paxutils, and use:

    scanelf -e ./litecoinz

    The output should contain:

     TYPE
    ET_DYN

* Non-executable Stack
    If the stack is executable then trivial stack-based buffer overflow exploits are possible if
    vulnerable buffers are found. By default, LitecoinZ Core should be built with a non-executable stack,
    but if one of the libraries it uses asks for an executable stack or someone makes a mistake
    and uses a compiler extension which requires an executable stack, it will silently build an
    executable without the non-executable stack protection.

    To verify that the stack is non-executable after compiling use:
    `scanelf -e ./litecoinz`

    The output should contain:
	STK/REL/PTL
	RW- R-- RW-

    The STK RW- means that the stack is readable and writeable but not executable.

Disable-wallet mode
--------------------
When the intention is to run only a P2P node without a wallet, LitecoinZ Core may be compiled in
disable-wallet mode with:

    ./configure --disable-wallet

Mining is also possible in disable-wallet mode, but only using the `getblocktemplate` RPC
call not `getwork`.

Additional Configure Flags
--------------------------
A list of additional configure flags can be displayed with:

    ./configure --help

For further documentation on the depends system see [README.md](../depends/README.md) in the depends directory.

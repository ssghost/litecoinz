Mac OS X Build Instructions and Notes
====================================
The commands in this guide should be executed in a Terminal application.
The built-in one is located in `/Applications/Utilities/Terminal.app`.

Preparation
-----------

Install the OS X command line tools:

`xcode-select --install`

When the popup appears, click `Install`.

Then install [Homebrew](https://brew.sh).

Dependencies
----------------------

    brew install autoconf automake libtool boost openssl pkg-config protobuf python libevent 

Build LitecoinZ Core
------------------------

1. Clone the litecoinz source code and cd into `litecoinz`

        git clone https://github.com/litecoinz-project/litecoinz.git
        cd litecoinz

2.  Build litecoinz-core:

        ./zcutil/build-darwin.sh

For further documentation on the depends system see [README.md](../depends/README.md) in the depends directory.

Running
-------

LitecoinZ Core is now available at `./src/litecoinzd`

Before running, it's recommended you create an RPC configuration file.

    echo -e "rpcuser=litecoinzrpc\nrpcpassword=$(xxd -l 16 -p /dev/urandom)" > "/Users/${USER}/Library/Application Support/LitecoinZ/litecoinz.conf"
    chmod 600 "/Users/${USER}/Library/Application Support/LitecoinZ/litecoinz.conf"

The first time you run litecoinzd, it will start downloading the blockchain. This process could take several hours.

You can monitor the download process by looking at the debug.log file:

    tail -f $HOME/Library/Application\ Support/LitecoinZ/debug.log

Other commands:
-------

    ./litecoinzd -daemon # to start the litecoinz daemon.
    ./litecoinz-cli --help  # for a list of command-line options.
    ./litecoinz-cli help    # When the daemon is running, to get a list of RPC commands

Notes
-----

* Tested on OS X 10.8 through 10.13 on 64-bit Intel processors only.

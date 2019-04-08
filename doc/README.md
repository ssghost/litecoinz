LitecoinZ Core
=============

Setup
---------------------
LitecoinZ Core is the original LitecoinZ client and it builds the backbone of the network. It downloads and, by default, stores the entire history of LitecoinZ transactions (which is currently more than 7 GBs); depending on the speed of your computer and network connection, the synchronization process can take anywhere from a few hours to a day or more.

To download LitecoinZ Core, visit [litecoinz.org](https://litecoinz.org).

Running
---------------------
The following are some helpful notes on how to run LitecoinZ Core on your native platform.

### Unix

Unpack the files into a directory and run:

- `bin/litecoinz-qt` (GUI) or
- `bin/litecoinzd` (headless)

### Windows

Unpack the files into a directory, and then run litecoinz-qt.exe.

### macOS

Drag LitecoinZ Core to your applications folder, and then run LitecoinZ Core.

### Need Help?

* Ask for help on the [Litecoinz Discord](https://discord.gg/QNcS4Pm) channel.

Building
---------------------
The following are developer notes on how to build LitecoinZ Core on your native platform. They are not complete guides, but include notes on the necessary libraries, compile flags, etc.

- [Dependencies](dependencies.md)
- [macOS Build Notes](build-osx.md)
- [Unix Build Notes](build-unix.md)
- [Windows Build Notes](build-windows.md)
- [Gitian Building Guide](gitian-building.md)

Development
---------------------
The LitecoinZ repo's [root README](/README.md) contains relevant information on the development process and automated testing.

- [Developer Notes](developer-notes.md)
- [Release Notes](release-notes.md)
- [Release Process](release-process.md)
- [Translation Process](translation_process.md)
- [Translation Strings Policy](translation_strings_policy.md)
- [Travis CI](travis-ci.md)
- [Dnsseed Policy](dnsseed-policy.md)

### Resources
* Discuss on the [Litecoinz Discord](https://discord.gg/QNcS4Pm) channel.

### Miscellaneous
- [Assets Attribution](assets-attribution.md)
- [Files](files.md)
- [Reduce Traffic](reduce-traffic.md)
- [Tor Support](tor.md)
- [Init Scripts (systemd/upstart/openrc)](init.md)
- [ZMQ](zmq.md)

License
---------------------
Distributed under the [MIT software license](/COPYING).
This product includes software developed by the OpenSSL Project for use in the [OpenSSL Toolkit](https://www.openssl.org/). This product includes
cryptographic software written by Eric Young ([eay@cryptsoft.com](mailto:eay@cryptsoft.com)), and UPnP software written by Thomas Bernard.

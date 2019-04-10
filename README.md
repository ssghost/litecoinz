LitecoinZ Core integration/staging tree
=====================================

https://litecoinz.org

What is LitecoinZ?
----------------

LitecoinZ is an experimental digital currency that enables instant payments to
anyone, anywhere in the world. LitecoinZ uses peer-to-peer technology to operate
with no central authority: managing transactions and issuing money are carried
out collectively by the network. LitecoinZ Core is the name of open source
software which enables the use of this currency.

[LitecoinZ](https://litecoinz.org/) is an implementation of the "[Zerocash](https://github.com/zcash/zips/raw/master/protocol/protocol.pdf)" protocol forked from [ZCash](https://z.cash/).
Based on Bitcoin's code, it intends to offer a far higher standard of privacy
through a sophisticated zero-knowledge proving scheme that preserves
confidentiality of transaction metadata. 

For more information, as well as an immediately useable, binary version of
the LitecoinZ Core software, see [https://litecoinz.org](https://litecoinz.org).

License
-------

LitecoinZ Core is released under the terms of the MIT license. See [COPYING](COPYING) for more
information or see https://opensource.org/licenses/MIT.

Development Process
-------------------

The `master` branch is regularly built and tested, but is not guaranteed to be
completely stable. [Tags](https://github.com/litecoinz-project/litecoinz/tags) are created
regularly to indicate new official, stable release versions of LitecoinZ Core.

The contribution workflow is described in [CONTRIBUTING.md](CONTRIBUTING.md).

Testing
-------

Testing and code review is the bottleneck for development; we get more pull
requests than we can review and test on short notice. Please be patient and help out by testing
other people's pull requests, and remember this is a security-critical project where any mistake might cost people
lots of money.

### Automated Testing

Developers are strongly encouraged to write [unit tests](src/test/README.md) for new code, and to
submit new unit tests for old code. Unit tests can be compiled and run
(assuming they weren't disabled in configure) with: `make check`. Further details on running
and extending unit tests can be found in [/src/test/README.md](/src/test/README.md).

The Travis CI system makes sure that every pull request is built for Windows, Linux, and macOS, and that unit/sanity tests are run automatically.

### Manual Quality Assurance (QA) Testing

Changes should be tested by somebody other than the developer who wrote the
code. This is especially important for large or high-risk changes. It is useful
to add a test plan to the pull request description if testing the changes is
not straightforward.

Translations
------------

We only accept translation fixes that are submitted through [Bitcoin Core's Transifex page](https://www.transifex.com/projects/p/bitcoin/).
Translations are converted to LitecoinZ periodically.

Translations are periodically pulled from Transifex and merged into the git repository. See the
[translation process](doc/translation_process.md) for details on how this works.

**Important**: We do not accept translation changes as GitHub pull requests because the next
pull from Transifex would automatically overwrite them again.

Donations
---------

Donations for supporting developers are welcome: 

- LTC: LgSbGGqru5LVHE3cWcgwZDLjr3WorGahr3
- BTC: 1N2rQimKbeUQA8N2LU5vGopYQJmZsBM2d6
- ZEC: t1T8hAYzLNwe2rLQpFMjx9z9FHFd6oQ86P9
- BTG: GNkwW2uS4ed7cofz94J8PrTXJnZgmk5nr2
- BTCZ: t1cmr2QUMCEBRhUVrZHsgC6DnrdzC2FyHz7

Thanks

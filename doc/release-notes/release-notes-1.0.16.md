Notable changes
===============

Change equihash parameters
--------------------------

This release change the equihash parameter on block 95,000 from n=200,k=9 to
n=144,k=5. Changing these parameters in the equihash algorithm requires much
more RAM (or GPU memory), making ASIC devices not economically viable.

`-disabledeprecation` removal
-----------------------------

This release removes the `-disabledeprecation` option, so that `litecoinzd` versions
will always shut down in accordance with the defined deprecation policy (currently 16
weeks after release). Users who wish to use a different policy must now specifically
choose to either:

- edit and compile the source code themselves, or
- obtain a software version from someone else who has done so (and obtain
  support from them).

Either way, it is much clearer that the software they are running is not
supported by the LitecoinZ Team.

Changelog
=========

* d42544f Give a better error message if system clock is bad
* 8478c71 Fix parsing parameters in getnetworksolps
* 6f4d27f Remove config option -disabledeprecation
* 501bb23 Modify the APPROX_RELEASE_HEIGHT.
* 610e78c Update version to 1.0.16
* 5e4da1a Update checkpoints
* 3349e00 Halving the difficulty for the algorithm change
* b7ff4b8 Update network protocol version
* d1f5dfe Change equihash parameters for mainnet/testnet (n=144,k=5).
* edc9fa1 Use std::thread::hardware_concurrency instead of Boost, to determine available cores
* 2650db5 Change equihash parameters for mainnet/testnet (n=192,k=7) and regtest (n=96,k=5)
* 89be84c New LitecoinZ logo
* 52cc2fb New gegtest genesis block
* cf658c5 Fix typo
* 9cbba95 Update intro.cpp file
* 6097f95 Update some doc files
* d7595c6 Windows cross-compile support

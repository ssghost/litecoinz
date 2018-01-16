LitecoinZ 1.0.14
================

What is LitecoinZ?
------------------

[LitecoinZ](https://litecoinz.info/) is an implementation of the "[Zerocash](https://github.com/zcash/zips/raw/master/protocol/protocol.pdf)" protocol forked from [ZCash](https://z.cash/).
Based on Bitcoin's code, it intends to offer a far higher standard of privacy
through a sophisticated zero-knowledge proving scheme that preserves
confidentiality of transaction metadata. 

This software is the LitecoinZ client. It downloads and stores the entire history
of LitecoinZ transactions; depending on the speed of your computer and network
connection, the synchronization process could take a day or more once the
blockchain has reached a significant size.

License
-------

LitecoinZ is released under the terms of the MIT license.

See [LICENSE](LICENSE) for more information or see [http://opensource.org/licenses/MIT](http://opensource.org/licenses/MIT).

Get started
-----------

## Linux

### Install dependencies

On Ubuntu/Debian-based systems:
```{r, engine='bash'}
$ sudo apt-get install \
      build-essential pkg-config libc6-dev m4 g++-multilib \
      autoconf libtool ncurses-dev unzip git python \
      zlib1g-dev wget bsdmainutils automake
```

On Fedora-based systems:
```{r, engine='bash'}
$ sudo dnf install \
      git pkgconfig automake autoconf ncurses-devel python \
      python-zmq wget gtest-devel gcc gcc-c++ libtool patch
```

### Fetch the software and parameter files

Fetch our repository with git and run ```fetch-params.sh``` like so:
```{r, engine='bash'}
$ git clone https://github.com/litecoinz-project/litecoinz.git
$ cd litecoinz/
$ git checkout v1.0.14
$ ./zcutil/fetch-params.sh
```

### Build

Ensure you have successfully installed all system package dependencies as described above. Then run the build, e.g.:
```{r, engine='bash'}
$ ./zcutil/build.sh -j$(nproc)
```

This should compile our dependencies and build ```litecoinzd```. (Note: if you don't have ```nproc```, then substitute the number of cores on your system. If the build runs out of memory, try again without the ```-j``` argument, i.e. just ```./zcutil/build.sh```. )

### Configuration

Create the ```~/.litecoinz``` directory and place a configuration file at ```~/.litecoinz/litecoinz.conf``` using the following commands:
```{r, engine='bash'}
$ mkdir -p ~/.litecoinz
$ echo "addnode=88.86.186.158" > ~/.litecoinz/litecoinz.conf
$ echo "rpcuser=username" >> ~/.litecoinz/litecoinz.conf
$ echo "rpcpassword=`head -c 32 /dev/urandom | base64`" >> ~/.litecoinz/litecoinz.conf
```

### Enabling CPU mining:

If you want to enable CPU mining, run these commands:

```{r, engine='bash'}
$ echo 'gen=1' >> ~/.litecoinz/litecoinz.conf
$ echo "genproclimit=-1" >> ~/.litecoinz/litecoinz.conf
```

Setting ```genproclimit=-1``` mines on the maximum number of threads possible on your CPU. If you want to mine with a lower number of threads, set ```genproclimit``` equal to the number of threads you would like to mine on.

### Running LitecoinZ:

Now, run litecoinzd!

```{r, engine='bash'}
$ ./src/litecoinzd
```

To run it in the background (without the node metrics screen that is normally displayed) use ```./src/litecoinzd --daemon```.

You should be able to use the RPC after it finishes loading. Here's a quick way to test:

```{r, engine='bash'}
$ ./src/litecoinz-cli getinfo
```

To see the peers you are connected to:
```{r, engine='bash'}
$ ./src/litecoinz-cli getpeerinfo
```

### Additional operations for litecoinz-cli

As LitecoinZ is an extension of bitcoin, litecoinz-cli supports all commands that are part of the Bitcoin Core API (as of version 0.11.2), [https://en.bitcoin.it/wiki/Original_Bitcoin_client/API_calls_list](https://en.bitcoin.it/wiki/Original_Bitcoin_client/API_calls_list)

To list all LitecoinZ commands, use ```./src/litecoinz-cli help```.

To get help with a particular command, use ```./src/litecoinz-cli help <command>```.


### Windows

- Download Windows release binary [here](https://litecoinz.info/downloads/litecoinz-1014-win64.zip)
- Fetch parameter files [here](https://litecoinz.info/downloads/sprout.zip) and uncompress the content inside the folder ```"C:\Users\%username%\AppData\Roaming\LitecoinZParams"```
- Manually edit the ```litecoinz.conf``` file inside the folder ```"C:\Users\%username%\AppData\Roaming\LitecoinZ"```
- Run ```litecoinzd.exe``` 


### Do you need a Wallet GUI?

A Java Wallet GUI is available [here](https://github.com/litecoinz-project/litecoinz-wallet).


Security Warnings
-----------------

**LitecoinZ is experimental and a work-in-progress.** Use at your own risk.

Participation in the LitecoinZ project is subject to a
[Code of Conduct](code_of_conduct.md).

TO DO
-----
- QT Wallet
- Backports from Bitcoin 0.12+

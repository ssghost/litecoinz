// Copyright (c) 2013 The Bitcoin Core developers
// Copyright (c) 2017-2018 The LitecoinZ developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <boost/test/unit_test.hpp>

#include "base58.h"
#include "key.h"
#include "uint256.h"
#include "util.h"
#include "utilstrencodings.h"
#include "test/test_bitcoin.h"

#include <string>
#include <vector>

struct TestDerivation {
    std::string pub;
    std::string prv;
    unsigned int nChild;
};

struct TestVector {
    std::string strHexMaster;
    std::vector<TestDerivation> vDerive;

    TestVector(std::string strHexMasterIn) : strHexMaster(strHexMasterIn) {}

    TestVector& operator()(std::string pub, std::string prv, unsigned int nChild) {
        vDerive.push_back(TestDerivation());
        TestDerivation &der = vDerive.back();
        der.pub = pub;
        der.prv = prv;
        der.nChild = nChild;
        return *this;
    }
};

TestVector test1 =
  TestVector("000102030405060708090a0b0c0d0e0f")
    ("xpub661MyMwAqRbcFtXgS5sYJABqqG9YLmC4Q1Rdap9gSE8NqtwybGhePY2gZ29ESFjqJoCu1Rupje8YtGqsefD265TMg7usUDFdp6W1EGMcet8",
     "xprv1ZKKpwtvh3uwNRVtYSCwtkXjGEdcBS19phprJJ7P1xKz1QhCxst4D3QpNtaoQEkqxoJptzzZibXzrRhs5ms5PfGaMTeeHWorP84GsbPLLZV",
     0x80000000)
    ("xpub68Gmy5EdvgibQVfPdqkBBCHxA5htiqg55crXYuXoQRKfDBFA1WEjWgP6LHhwBZeNK1VTsfTFUHCdrfp1bgwQ9xv5ski8PX9rL2dZXvgGDnw",
     "xprv1bajpfCPnK2vX2dbkC5amndqb4BxZWVAWKFkGPVVz9XGNgzPP7R9LBmEA9w8ox8ofrCXGJKS6xNsaBjajN9GBjM6RLKnXUWk68h8Jo3S465",
     1)
    ("xpub6ASuArnXKPbfEwhqN6e3mwBcDTgzisQN1wXN9BJcM47sSikHjJf3UFHKkNAWbWMiGj7Wf5uMash7SyYq527Hqck2AxYysAA7xmALppuCkwQ",
     "xprv1dks2SkHB1uzMUg3USyTNXXVeSB4ZYDTSdvarfGJvnKUcEVX6uqTHkfTaD8CQsur7vxD57usBfLHi8ForhwExtqXw7HWbEHbkw1VqfbEJkk",
     0x80000002)
    ("xpub6D4BDPcP2GT577Vvch3R8wDkScZWzQzMMUm3PWbmWvVJrZwQY4VUNgqFJPMM3No2dFDFGTsxxpG5uJh7n7epu4trkrX7x7DogT5Uv6fcLW5",
     "xprv1gN94ya8stmQDeU8j3NpjXZdsb3aq5oSnBAG6zZU6egv25gdufftCCDP8FLpP7RLdPybfarnc5ABwKzZSirS5zHCHir4kAXYNCkiUEkNzLf",
     2)
    ("xpub6FHa3pjLCk84BayeJxFW2SP4XRrFd1JYnxeLeU8EqN3vDfZmbqBqaGJAyiLjTAwm6ZLRQUMv1ZACTj37sR62cfN7fe5JnJ7dh8zL4fiyLHV",
     "xprv1ibXuQh64NSPJ7wrRJaud2iwxQLKTg7eDf3ZMx5wR6FXPBJzySNFPmgJoZkGqmpmhC7JHW6MWtcMQWu1e9PnHNsEgxwAJFMFXBDsyjvMkzp",
     1000000000)
    ("xpub6H1LXWLaKsWFhvm6RVpEL9P4KfRZSW7abD2ttkWP3SSQvnyA8FSVqNTEcYFgJS2UaFcxupHiYkro49S8yGasTvXEYBVPamhGW6cFJodrTHy",
     "xprv1kKJP6JLBVpapTjJXr9dvjiwkdudHAvg1uS7cEU5dAe26JiPVrcuesqNSRXUkgwrJbWZ4s5JMw9UQguPBDQWQLWn2ThPRA6xoWnEitLCBuy",
     0);

TestVector test2 =
  TestVector("fffcf9f6f3f0edeae7e4e1dedbd8d5d2cfccc9c6c3c0bdbab7b4b1aeaba8a5a29f9c999693908d8a8784817e7b7875726f6c696663605d5a5754514e4b484542")
    ("xpub661MyMwAqRbcFW31YEwpkMuc5THy2PSt5bDMsktWQcFF8syAmRUapSCGu8ED9W6oDMSgv6Zz8idoc4a6mr8BDzTJY47LJhkJ8UB7WEGuduB",
     "xprv1ZKKpwtvh3uwN31DebHELxFVWRn2s4FyWHcabErCzLSrJPiQ92ezdwaQixMrakJwCr4FeHT1zhaBwxRy8CFn12bLositRriaxNgWEa4iXsY",
     0)
    ("xpub69H7F5d8KSRgmmdJg2KhpAK8SR3DjMwAdkxj3ZuxV27CprR9LgpeyGmXUbC6wb7ERfvrnKZjXoUmmDznezpbZb7ap6r1D3tgFxHmwMkQTPH",
     "xprv1cb56fatB4k1tJbWnNf7Qkf1sPXHa2kG4TMwm3sf4kJozNANiJ14nn9fJTdfDp9mcWJ5YHrkRrzKJU4ngUkpwFkjp48wpLtL5nb4brLEBeK",
     0xFFFFFFFF)
    ("xpub6ASAVgeehLbnwdqV6UKMHVzgqAG8Gr6riv3Fxxpj8ksbH9ebxaEyBLZ85ySDhKiLDBrQSARLq1uNRts8RuJiHjaDMBU4Zn9h8LZNnBC5y4a",
     "xprv1dk8MGcQYxv84AohCpekt6LaG8kC7Wux9cSUgSnRiV5CSfPqLBRNzqwFup7dx4tFWjcyKK4PUsVRjhoZQrMxWZCJEnNHarcasZvbNEWRNay",
     1)
    ("xpub6DF8uhdarytz3FWdA8TvFSvvAh8dP3283MY7p2V4SeE2wyWmG5mg5EwVvmdMVCQcoNJxGoWaU9DCWh89LojfZ537wTfunKau47EL2dhHKon",
     "xprv1gZ6mHbLicDK9nUqGUoKr3GobfchDhqDU3wLXWSm2NRe7VFzdgx5tkKdkcKFyn5wpgyg4c8mtRAe4QaiTjtLX31bNYu6RxayMNMKqxaxmfX",
     0xFFFFFFFE)
    ("xpub6ERApfZwUNrhLCkDtcHTcxd75RbzS1ed54G1LkBUHQVHQKqhMkhgbmJbZRkrgZw4koxb5JaHWkY4ALHY2grBGRjaDMzQLcgJvLJuZZvRcEL",
     "xprv1hj8gFXhL1B2SjiRzxcsDYxzWQ64GgTiVkfE4E9As8gtZqavjMt6RGgjPK3ecCUENGGNNRumGnkDsXg5ygnqCsgmCVSQaAcbgxMquimBuMc",
     2)
    ("xpub6FnCn6nSzZAw5Tw7cgR9bi15UV96gLZhjDstkXXxvCLsUXBGXPdSnLFbdpq8p9HmGsApME5hQTZ3emM2rnY5agb9rXpVGyy3bdW6EEgAtqt",
     "xprv1j6AdgkCrBVGBzuKj2kZCJLxuTdAX1No9vH7U1VfVvYUe2vVtzorbqdjTiiemDZJQa86fBYaitauv5Zbi1A4FrRV1LwudakKPKhPzGhBs7T",
     0);

void RunTest(const TestVector &test) {
    std::vector<unsigned char> seed = ParseHex(test.strHexMaster);
    CExtKey key;
    CExtPubKey pubkey;
    key.SetMaster(&seed[0], seed.size());
    pubkey = key.Neuter();
    BOOST_FOREACH(const TestDerivation &derive, test.vDerive) {
        unsigned char data[74];
        key.Encode(data);
        pubkey.Encode(data);

        // Test private key
        CBitcoinExtKey b58key; b58key.SetKey(key);
        BOOST_CHECK(b58key.ToString() == derive.prv);

        CBitcoinExtKey b58keyDecodeCheck(derive.prv);
        CExtKey checkKey = b58keyDecodeCheck.GetKey();
        assert(checkKey == key); //ensure a base58 decoded key also matches

        // Test public key
        CBitcoinExtPubKey b58pubkey; b58pubkey.SetKey(pubkey);
        BOOST_CHECK(b58pubkey.ToString() == derive.pub);

        CBitcoinExtPubKey b58PubkeyDecodeCheck(derive.pub);
        CExtPubKey checkPubKey = b58PubkeyDecodeCheck.GetKey();
        assert(checkPubKey == pubkey); //ensure a base58 decoded pubkey also matches

        // Derive new keys
        CExtKey keyNew;
        BOOST_CHECK(key.Derive(keyNew, derive.nChild));
        CExtPubKey pubkeyNew = keyNew.Neuter();
        if (!(derive.nChild & 0x80000000)) {
            // Compare with public derivation
            CExtPubKey pubkeyNew2;
            BOOST_CHECK(pubkey.Derive(pubkeyNew2, derive.nChild));
            BOOST_CHECK(pubkeyNew == pubkeyNew2);
        }
        key = keyNew;
        pubkey = pubkeyNew;
    }
}

BOOST_FIXTURE_TEST_SUITE(bip32_tests, BasicTestingSetup)

BOOST_AUTO_TEST_CASE(bip32_test1) {
    RunTest(test1);
}

BOOST_AUTO_TEST_CASE(bip32_test2) {
    RunTest(test2);
}

BOOST_AUTO_TEST_SUITE_END()

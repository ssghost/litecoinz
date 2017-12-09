#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "chainparams.h"
#include "key.h"
#include "miner.h"
#include "util.h"
#ifdef ENABLE_WALLET
#include "wallet/wallet.h"
#endif

#include <boost/optional.hpp>

using ::testing::Return;

#ifdef ENABLE_WALLET
class MockReserveKey : public CReserveKey {
public:
    MockReserveKey() : CReserveKey(nullptr) { }

    MOCK_METHOD1(GetReservedKey, bool(CPubKey &pubkey));
};
#endif

TEST(Miner, GetMinerScriptPubKey) {
    SelectParams(CBaseChainParams::MAIN);

    boost::optional<CScript> scriptPubKey;
#ifdef ENABLE_WALLET
    MockReserveKey reservekey;
    EXPECT_CALL(reservekey, GetReservedKey(::testing::_))
        .WillRepeatedly(Return(false));
#endif

    // No miner address set
#ifdef ENABLE_WALLET
    scriptPubKey = GetMinerScriptPubKey(reservekey);
#else
    scriptPubKey = GetMinerScriptPubKey();
#endif
    EXPECT_FALSE((bool) scriptPubKey);

    mapArgs["-mineraddress"] = "notAnAddress";
#ifdef ENABLE_WALLET
    scriptPubKey = GetMinerScriptPubKey(reservekey);
#else
    scriptPubKey = GetMinerScriptPubKey();
#endif
    EXPECT_FALSE((bool) scriptPubKey);

    // Partial address
    mapArgs["-mineraddress"] = "L1SdUmxUpkrnRG5hBsdtX";
#ifdef ENABLE_WALLET
    scriptPubKey = GetMinerScriptPubKey(reservekey);
#else
    scriptPubKey = GetMinerScriptPubKey();
#endif
    EXPECT_FALSE((bool) scriptPubKey);

    // Typo in address
    mapArgs["-mineraddress"] = "LISdUmxUpkrnRG5hBsdtXzjonYtQ8sjcSYG";
#ifdef ENABLE_WALLET
    scriptPubKey = GetMinerScriptPubKey(reservekey);
#else
    scriptPubKey = GetMinerScriptPubKey();
#endif
    EXPECT_FALSE((bool) scriptPubKey);

    // Set up expected scriptPubKey for L1SdUmxUpkrnRG5hBsdtXzjonYtQ8sjcSYG
    CKeyID keyID;
    keyID.SetHex("62e72497b4319ac31d57f4668e437ab890e628cf");
    CScript expectedScriptPubKey = CScript() << OP_DUP << OP_HASH160 << ToByteVector(keyID) << OP_EQUALVERIFY << OP_CHECKSIG;

    // Valid address
    mapArgs["-mineraddress"] = "L1SdUmxUpkrnRG5hBsdtXzjonYtQ8sjcSYG";
#ifdef ENABLE_WALLET
    scriptPubKey = GetMinerScriptPubKey(reservekey);
#else
    scriptPubKey = GetMinerScriptPubKey();
#endif
    EXPECT_TRUE((bool) scriptPubKey);
    EXPECT_EQ(expectedScriptPubKey, *scriptPubKey);

    // Valid address with leading whitespace
    mapArgs["-mineraddress"] = "  L1SdUmxUpkrnRG5hBsdtXzjonYtQ8sjcSYG";
#ifdef ENABLE_WALLET
    scriptPubKey = GetMinerScriptPubKey(reservekey);
#else
    scriptPubKey = GetMinerScriptPubKey();
#endif
    EXPECT_TRUE((bool) scriptPubKey);
    EXPECT_EQ(expectedScriptPubKey, *scriptPubKey);

    // Valid address with trailing whitespace
    mapArgs["-mineraddress"] = "L1SdUmxUpkrnRG5hBsdtXzjonYtQ8sjcSYG  ";
#ifdef ENABLE_WALLET
    scriptPubKey = GetMinerScriptPubKey(reservekey);
#else
    scriptPubKey = GetMinerScriptPubKey();
#endif
    EXPECT_TRUE((bool) scriptPubKey);
    EXPECT_EQ(expectedScriptPubKey, *scriptPubKey);
}

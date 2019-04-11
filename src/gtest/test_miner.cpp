#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "chainparams.h"
#include "key.h"
#include "miner.h"
#include "util.h"


TEST(Miner, GetScriptForMinerAddress) {
    SelectParams(CBaseChainParams::MAIN);

    // No miner address set
    {
        boost::shared_ptr<CReserveScript> coinbaseScript;
        GetScriptForMinerAddress(coinbaseScript);
        EXPECT_FALSE((bool) coinbaseScript);
    }

    mapArgs["-mineraddress"] = "notAnAddress";
    {
        boost::shared_ptr<CReserveScript> coinbaseScript;
        GetScriptForMinerAddress(coinbaseScript);
        EXPECT_FALSE((bool) coinbaseScript);
    }

    // Partial address
    mapArgs["-mineraddress"] = "L1SdUmxUpkrnRG5hBsdtX";
    {
        boost::shared_ptr<CReserveScript> coinbaseScript;
        GetScriptForMinerAddress(coinbaseScript);
        EXPECT_FALSE((bool) coinbaseScript);
    }

    // Typo in address
    mapArgs["-mineraddress"] = "LISdUmxUpkrnRG5hBsdtXzjonYtQ8sjcSYG";
    {
        boost::shared_ptr<CReserveScript> coinbaseScript;
        GetScriptForMinerAddress(coinbaseScript);
        EXPECT_FALSE((bool) coinbaseScript);
    }

    // Set up expected scriptPubKey for L1SdUmxUpkrnRG5hBsdtXzjonYtQ8sjcSYG
    CKeyID keyID;
    keyID.SetHex("62e72497b4319ac31d57f4668e437ab890e628cf");
    CScript expectedCoinbaseScript = CScript() << OP_DUP << OP_HASH160 << ToByteVector(keyID) << OP_EQUALVERIFY << OP_CHECKSIG;

    // Valid address
    mapArgs["-mineraddress"] = "L1SdUmxUpkrnRG5hBsdtXzjonYtQ8sjcSYG";
    {
        boost::shared_ptr<CReserveScript> coinbaseScript;
        GetScriptForMinerAddress(coinbaseScript);
        EXPECT_TRUE((bool) coinbaseScript);
        EXPECT_EQ(expectedCoinbaseScript, coinbaseScript->reserveScript);
    }

    // Valid address with leading whitespace
    mapArgs["-mineraddress"] = "  L1SdUmxUpkrnRG5hBsdtXzjonYtQ8sjcSYG";
    {
        boost::shared_ptr<CReserveScript> coinbaseScript;
        GetScriptForMinerAddress(coinbaseScript);
        EXPECT_TRUE((bool) coinbaseScript);
        EXPECT_EQ(expectedCoinbaseScript, coinbaseScript->reserveScript);
    }

    // Valid address with trailing whitespace
    mapArgs["-mineraddress"] = "L1SdUmxUpkrnRG5hBsdtXzjonYtQ8sjcSYG  ";
    {
        boost::shared_ptr<CReserveScript> coinbaseScript;
        GetScriptForMinerAddress(coinbaseScript);
        EXPECT_TRUE((bool) coinbaseScript);
        EXPECT_EQ(expectedCoinbaseScript, coinbaseScript->reserveScript);
    }
}

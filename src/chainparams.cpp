// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "main.h"
#include "crypto/equihash.h"

#include "util.h"
#include "utilstrencodings.h"

#include <assert.h>

#include <boost/assign/list_of.hpp>

#include "base58.h"

using namespace std;

#include "chainparamsseeds.h"

/**
 * Main network
 */
/**
 * What makes a good checkpoint block?
 * + Is surrounded by blocks with reasonable timestamps
 *   (no blocks before with a timestamp after, none after with
 *    timestamp before)
 * + Contains no strange transactions
 */

const arith_uint256 maxUint = UintToArith256(uint256S("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"));

class CMainParams : public CChainParams {
public:
    CMainParams() {
        strNetworkID = "main";
        strCurrencyUnits = "LTZ";
        consensus.fCoinbaseMustBeProtected = true;
        consensus.nSubsidySlowStartInterval = 20000;
        consensus.nSubsidyHalvingInterval = 840000;
        consensus.nMajorityEnforceBlockUpgrade = 750;
        consensus.nMajorityRejectBlockOutdated = 950;
        consensus.nMajorityWindow = 4000;
        consensus.powLimit = uint256S("0007ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowAveragingWindow = 17;
        assert(maxUint/UintToArith256(consensus.powLimit) >= consensus.nPowAveragingWindow);
        consensus.nPowMaxAdjustDown = 32; // 32% adjustment down
        consensus.nPowMaxAdjustUp = 16; // 16% adjustment up
        consensus.nPowTargetSpacing = 2.5 * 60;
        /**
         * The message start string should be awesome! ⓩ❤
         */
        pchMessageStart[0] = 0xd8;
        pchMessageStart[1] = 0xcf;
        pchMessageStart[2] = 0xcd;
        pchMessageStart[3] = 0x93;
        vAlertPubKey = ParseHex("0445bd31cd04bf575c9b588bf9bc6f164e50737c489388ecc6dd417829fc5620ee1ec6b137279c93dee907a3ad348a79a4196c957ef8542c2ca4dc0a1da99933d8");
        nDefaultPort = 29333;
        nMaxTipAge = 24 * 60 * 60;
        nPruneAfterHeight = 100000;
        const size_t N = 200, K = 9;
        BOOST_STATIC_ASSERT(equihash_parameters_acceptable(N, K));
        nEquihashN = N;
        nEquihashK = K;

        /**
         * Build the genesis block. Note that the output of its generation
         * transaction cannot be spent since it did not originally exist in the
         * database (and is in any case of zero value).
         *
         * >>> from pyblake2 import blake2s
         * >>> 'LitecoinZ' + blake2s(b'NY Times 05/Oct/2011 Steve Jobs, Apple’s Visionary, Dies at 56LTC#433272 27659f4c97de825afe9f1d4ab6c2ba4e83751a11f0144905b0fec849c36fb1ce LTC#741825 27659c79fbb898e9c35f55d49126afcc3a63d1501d399651f21980c538cbaa8f DJIA close on 29 Nov 2017: 23,940.68').hexdigest()
         */
        const char* pszTimestamp = "LitecoinZ6f099db24212fc48cc80bcf8d46874cd44a7a196625cc1e08a567bff77da2e79";
        CMutableTransaction txNew;
        txNew.nVersion = 1;
        txNew.vin.resize(1);
        txNew.vout.resize(1);
        txNew.vin[0].scriptSig = CScript() << 486604799 << CScriptNum(4) << vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
        txNew.vout[0].nValue = 0;
        txNew.vout[0].scriptPubKey = CScript() << ParseHex("04bccfb90b0364ac381985755cb15f2e14214dd9239de85d9ff58bf2eed145dc776daa3469b29b50e459939df5346da0d6d40dbc698830f2022fe64ed6aa701f74") << OP_CHECKSIG;
        genesis.vtx.push_back(txNew);
        genesis.hashPrevBlock.SetNull();
        genesis.hashMerkleRoot = genesis.BuildMerkleTree();
        genesis.nVersion = 4;
        genesis.nTime    = 1512089155;
        genesis.nBits    = 0x1f07ffff;
        genesis.nNonce   = uint256S("0x00000000000000000000000000000000000000000000000000000000000012d8");
        genesis.nSolution = ParseHex("00f87e14bbddec873b739169176e6755eac61b45eb24e56a66074f7eaa9fc965b919640ba3322cbd336d051bc1dfc68acf12fff3d47f17a958a578ea16f2d41391332a789472d1fa35a2071e24f65a5c8e9c577d1161069dd129d57d743128c376e0b17a3a4175694011e628cb6c6096551bb831d6c99e9250e4a6b39aae147223cf826de36fe08a21a11563ea29392a31cd9629db03ccab29d97785ddc38aea9e9e81db24db4b071107e991509d84a9810b37450a5cf7a6978e5a0e0a1b92454136edbcb3d37fb2dbb9f77d75608fd89a8314d6b436cdd2fb7fba8355a6df6ca81e6e26bf3302186b740d5895cd9542f454454f7b127551fed3bd8c189e39e6d07110fffc9de3757e3d37719fbd5b6790408044c1f6a507d9b6722a70adf3452ed7239c045c3486cda793926325a547c84707f58b83c23abe73b43c106beb2697cd59ce30c51687aafd09be651376a902d234ac13d9a7b7b8dbf42f31d2878d5c8b93bd1e0e06814654dfe69df54501fa034faaa5c06170fd6b0efa85fc97c8c1df1a9541a9b85853df32c15ff67015eeca87612a68118c35d727ea61cf55e85afd1b90171f08d94391d909683ac2e3f39812016458fbede82cdb9caf00958f4bc1fc74e736ffd371bf7698d6aa17386e1239eba8f5dc67c45c81bb8266031af902203e4ecd778420b9873dc7a4a29743eef5c98e36c5e909ab105f2378e721fbecb287ae1db688f9c8be5e322272af5c810e795b1dadc482d6cd34613e95d581980ef5e9a391d07d72e964a65026fe17ca98fa5b414f0faf3db8bbc46bc312e081ee55166241928b5f419c0ed2039b996f3bbf85aa469b20e3cea3c2fabf2fe00f85670ee7ab4febdf1dc3ef1a290239b997ef45172be6775fbc1aa9b17035f41f19c5de2170741a753034c1d240f091d6aaf88935c77ff86c57cba19f105a022eaba95d4c2d4f067fc117a691eb165371dffa8739cc49d98e14c2158264946f1d36b1ba34579ec7940524a428f29efaa9fd1c4539b970bad1b2595585060a220e4102850e19ff165121ddebbab8e4c49c42f80ed6b8bd470d370e6f56a79dfcf3bdef5d52beff9e15524f964dad0b499ce60391d2b0c45e922ad65e4e24ab39ac84633c2dcbc7da6e6c5efdcfb2957dfc892bb03746afb19611c95355a52440d5ba3d61b87ff602a34910e394315f313900573e2d1279e9b899bce75d4e3e14333175b7f76cb8a35ee0893320bb1aa7410f293175285706d7358523dec7f37bed56b5714aaf182c9a9058083974e955a6080b471ed9f6f07c0f5f1619e92a7ba4599ddf6916b4b37293f5e06eb443763af4f535e49e8e47ded269867cfed8a666df74e61c17991788df89fd60e0f422d17af89d29e8b23dd33631488f4bb3a70d17b3e2246f18db620749873e946a094d9ad35f492627ae95e9836ae3399acfb7171a501a8f6f431c5625a5e4ff431af1367ba4e3d35de414138f9a9f12861fa8be65f171bd75a39e3c66bd03911ebca289005d5e28f18c4347a3df6164d799d569752649f5dbb409e99cce5592f1055afa71dc90bf40492fafd4ba6210180558dcf568146b0eaa36933550e2273dfb76b70f836b6f6f5673aaf0bb865470391cc24343b34e0a52143390991638b3b48bf1924fbfe9fc104f1cd8588c34b9c43041beed92a22d9a0674aa6b3868738f9257186fa5339649535f255a110512c15214d11ab9db4f739167a45819d47b63ea7e2c1804b3374b3ab5746510fbbb53b4e01e43f86a5d03f3ea07227205fdfecb9a96c918d65cef3ba701a325904da952045375d599d97d1f7b9525bbe9af06e58e5c7c7233d99bcf5c4e93716ba4534d00b9bda9ac63df9d9f3a8b928804db05b533f0e42009bfc7ee15f0b6470a");

        consensus.hashGenesisBlock = genesis.GetHash();
        assert(genesis.hashMerkleRoot == uint256S("0xf67057581d7f6b4316cd854aac32d14228f9d9ed92125317eeb2efbadc395d01"));
        assert(consensus.hashGenesisBlock == uint256S("0x00004a67df5661dc6d1fcb6e66edc27c1db7470826bf0f0c27123931a90075ae"));


        vFixedSeeds.clear();
        vSeeds.clear();
        vSeeds.push_back(CDNSSeedData("litecoinz.info", "dnsseed.litecoinz.info")); // LitecoinZ

        // guarantees the first 2 characters, when base58 encoded, are "t1"
        base58Prefixes[PUBKEY_ADDRESS]     = {0x1C,0xB8};
        // guarantees the first 2 characters, when base58 encoded, are "t3"
        base58Prefixes[SCRIPT_ADDRESS]     = {0x1C,0xBD};
        // the first character, when base58 encoded, is "5" or "K" or "L" (as in Bitcoin)
        base58Prefixes[SECRET_KEY]         = {0x80};
        // do not rely on these BIP32 prefixes; they are not specified and may change
        base58Prefixes[EXT_PUBLIC_KEY]     = {0x04,0x88,0xB2,0x1E};
        base58Prefixes[EXT_SECRET_KEY]     = {0x04,0x88,0xAD,0xE4};
        // guarantees the first 2 characters, when base58 encoded, are "zc"
        base58Prefixes[ZCPAYMENT_ADDRRESS] = {0x16,0x9A};
        // guarantees the first 2 characters, when base58 encoded, are "SK"
        base58Prefixes[ZCSPENDING_KEY]     = {0xAB,0x36};

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_main, pnSeed6_main + ARRAYLEN(pnSeed6_main));

        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = false;

        checkpointData = (Checkpoints::CCheckpointData) {
            boost::assign::map_list_of
            (0, consensus.hashGenesisBlock),
            1512089155,     // * UNIX timestamp of last checkpoint block
            1,              // * total number of transactions between genesis and last checkpoint
                            //   (the tx=... number in the SetBestChain debug.log lines)
            2777            // * estimated number of transactions per day after checkpoint
                            //   total number of tx / (checkpoint block height / (24 * 24))
        };

        // Founders reward script expects a vector of 2-of-3 multisig addresses
        vFoundersRewardAddress = { };
        //assert(vFoundersRewardAddress.size() <= consensus.GetLastFoundersRewardBlockHeight());
    }
};
static CMainParams mainParams;

/**
 * Testnet (v3)
 */
class CTestNetParams : public CMainParams {
public:
    CTestNetParams() {
        strNetworkID = "test";
        strCurrencyUnits = "TLZ";
        consensus.nMajorityEnforceBlockUpgrade = 51;
        consensus.nMajorityRejectBlockOutdated = 75;
        consensus.nMajorityWindow = 400;
        consensus.powLimit = uint256S("07ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        assert(maxUint/UintToArith256(consensus.powLimit) >= consensus.nPowAveragingWindow);
        pchMessageStart[0] = 0xfe;
        pchMessageStart[1] = 0x90;
        pchMessageStart[2] = 0x86;
        pchMessageStart[3] = 0x5d;
        vAlertPubKey = ParseHex("04bccfb90b0364ac381985755cb15f2e14214dd9239de85d9ff58bf2eed145dc776daa3469b29b50e459939df5346da0d6d40dbc698830f2022fe64ed6aa701f74");
        nDefaultPort = 39333;
        nPruneAfterHeight = 1000;

        //! Modify the testnet genesis block so the timestamp is valid for a later start.
        genesis.nTime = 1511954736;
        genesis.nBits = 0x1f07ffff;
        genesis.nNonce = uint256S("0x0000000000000000000000000000000000000000000000000000000000000cc3");
        genesis.nSolution = ParseHex("007fc5fa1c902f6f3d48c00e3605dc0903137de3270eb83ef6cd968b1f03b2d60f7c367a4eab5878ae210e5cd2cf3a6d4e23b75f644e7128a8d63231b5c30c2d082a526da38bedcb1fd6cc6dfee5e267d5b65f4505b0502e6c16093790890533d2bafc0240ec18e5e63e740f15e192fec5c974164746fa47da49f17c2e6008ba011b96194b70cab95ceb10fc98375e88be503e0e13c31c1c9934cd0309511aac47e97d14270f8a2b010c122d85a85a837b990271a897ab959084fa349a0f60cdca2d056adbea98915b0729a94d5f0af6cad620028f065d5b4df7fab7263ba6422f91c64932f40d30542b58978fd817f27b63858976d4f277e9fc16ef06c93dc1c2088115f710fb7410f2e62321367bd09d6016c64f882f1071c5965662b33792f6fe6eb839d92b8b339a43218c5b66be33a4c352f9b36fd05cd4a550efb5a138e4c3e3b613696a33f2e0ae5d1fb532e1070e72875d6a5e71c8c1f3e719446a11797d0bd08d2137da549f5e7b295dcb5339f3c7e74cee41b192290eba49b72908be53382e3728d06cd5828f117c66c2146dc4130510f8db8822e1d9f45d89226bd6f9cc8815492353a01b83217c7e732405f4812160be9e2515242b1a76670c4e711f37a6eaa4490c39e61f3acba61660925b5e0d7d8cdce651cdc719d7d0da8a8774351bb4b200076580dd2e3642f6051a700d61cf58a5e60c15d4cd51cd968b06770aeb1e7b3813596d5d7604250e5b1b7c1f31b5d946682d1d64d4331be1b9d2e20cce30e58acde07cad4da552703fbcb1962c31a3a40e1c8a38e8d0d391396b510c61c093829e23bdbe78155f8153e5cf3c530fde82dcb4569ef657d73d72f01b907d5869109cfcb68ef1dc34351595b5784e07ad1c747b1f7cef2b71c18e821c95729d4d38acae9ebb1da4fdb32dcf08779fe016e0a0589a8384b57ed95d00b55fb9e5ca03527362066663e632da1744b4a6c40421223c7bc37f475c1a00eae6b76c79011e1a5fea2a6f36b9c96e48bd74c593217ce405a3dd441f000a7a09d65ac2afaea1fe9439fc3c78657a95009d7e670a2d4e8c14e34577896c8227add2071dc4e26f1e331a905420ac0cea42b98262d1a0f2c22a2a8aff5f2220c1f1fa81d6b67d57b176c196faa1e37c973fa846425175cc0f318eabf1ee76ce3c4f797737e3fe6c4200c893ac2108ff01a35b7460fdd2290f14bb3d3caf05922c274189b3590cbf20f4c93ad0931ff37f8961022f9a14cc0badd157dd4837c76b58a2ef5e7fa1c61dea135ea24a0c0fa4264496fe37046636a7599ad6064a150baca6686f889d04a91dd55a02265b57ae3c395af674f55425218d7424f77034af1b91b91e4516179e82bfe4cbc3058532a62e02eca88e4abd3c9e526e2d0c55d7299117f965ba91557810eec4671de300056c76dc8143c41a425d71464467555b38eb9b98e01584f951839923c5466374a6782f2285a789550fa508436efbe9c6b6086e9104687a32fe3ae4bd9e35fd18138774a7fc7339f96b2387122c8dfa90e75642dd077caa4f58494565bc4451f9ddec86a8b864be83603390fd3b2450c92fbf725491505347015ec63e61570b4a8c718f226451b0825b1d4575d9eacea11ce26f1daa656e38078a23f4102275e8fef1b98c31919ed80a77b9472d611af54629c167d04ee176b1c1d855000ef3c970fbfa5695ec32b59e5131d0df1fd1f9b55717d12bd6ef9e66cf466a96429b775e3a466b135cb7220041dad98cb19150ec243c6650114986efdd1b2a2fe6b7bb914db476b2ba63e3f6e50dbdaccc563bed918cbca87eab2a3be67f5a73606957df05a9bd0a923f7df6d49cd0b7759e00355d494a2b66c14098f3fa5a36560306d95bf9c3c36642927e30ede7f89a7e90");

        consensus.hashGenesisBlock = genesis.GetHash();
        assert(genesis.hashMerkleRoot == uint256S("0xf67057581d7f6b4316cd854aac32d14228f9d9ed92125317eeb2efbadc395d01"));
        assert(consensus.hashGenesisBlock == uint256S("0x00004cab212604126942d64c240bdb0c26aa9f546f70bf8d46db3445c7ab254d"));

        vFixedSeeds.clear();
        vSeeds.clear();
        vSeeds.push_back(CDNSSeedData("litecoinz.info", "dnsseed.litecoinz.info")); // LitecoinZ

        // guarantees the first 2 characters, when base58 encoded, are "tm"
        base58Prefixes[PUBKEY_ADDRESS]     = {0x1D,0x25};
        // guarantees the first 2 characters, when base58 encoded, are "t2"
        base58Prefixes[SCRIPT_ADDRESS]     = {0x1C,0xBA};
        // the first character, when base58 encoded, is "9" or "c" (as in Bitcoin)
        base58Prefixes[SECRET_KEY]         = {0xEF};
        // do not rely on these BIP32 prefixes; they are not specified and may change
        base58Prefixes[EXT_PUBLIC_KEY]     = {0x04,0x35,0x87,0xCF};
        base58Prefixes[EXT_SECRET_KEY]     = {0x04,0x35,0x83,0x94};
        // guarantees the first 2 characters, when base58 encoded, are "zt"
        base58Prefixes[ZCPAYMENT_ADDRRESS] = {0x16,0xB6};
        // guarantees the first 2 characters, when base58 encoded, are "ST"
        base58Prefixes[ZCSPENDING_KEY]     = {0xAC,0x08};

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_test, pnSeed6_test + ARRAYLEN(pnSeed6_test));

        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = true;

        checkpointData = (Checkpoints::CCheckpointData) {
            boost::assign::map_list_of
            (0, consensus.hashGenesisBlock),
            1511954736,  // * UNIX timestamp of last checkpoint block
            1,           // * total number of transactions between genesis and last checkpoint
                         //   (the tx=... number in the SetBestChain debug.log lines)
            715          //   total number of tx / (checkpoint block height / (24 * 24))
        };

        // Founders reward script expects a vector of 2-of-3 multisig addresses
        vFoundersRewardAddress = { };
        //assert(vFoundersRewardAddress.size() <= consensus.GetLastFoundersRewardBlockHeight());
    }
};
static CTestNetParams testNetParams;

/**
 * Regression test
 */
class CRegTestParams : public CTestNetParams {
public:
    CRegTestParams() {
        strNetworkID = "regtest";
        strCurrencyUnits = "RLZ";
        consensus.fCoinbaseMustBeProtected = false;
        consensus.nSubsidySlowStartInterval = 0;
        consensus.nSubsidyHalvingInterval = 150;
        consensus.nMajorityEnforceBlockUpgrade = 750;
        consensus.nMajorityRejectBlockOutdated = 950;
        consensus.nMajorityWindow = 1000;
        consensus.powLimit = uint256S("0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f");
        assert(maxUint/UintToArith256(consensus.powLimit) >= consensus.nPowAveragingWindow);
        consensus.nPowMaxAdjustDown = 0; // Turn off adjustment down
        consensus.nPowMaxAdjustUp = 0; // Turn off adjustment up
        pchMessageStart[0] = 0xea;
        pchMessageStart[1] = 0x8c;
        pchMessageStart[2] = 0x71;
        pchMessageStart[3] = 0x19;
        nMaxTipAge = 24 * 60 * 60;
        const size_t N = 48, K = 5;
        BOOST_STATIC_ASSERT(equihash_parameters_acceptable(N, K));
        nEquihashN = N;
        nEquihashK = K;
        genesis.nTime = 1511954736;
        genesis.nBits = 0x200f0f0f;
        genesis.nNonce = uint256S("0x0000000000000000000000000000000000000000000000000000000000000000");
        genesis.nSolution.clear();

        consensus.hashGenesisBlock = genesis.GetHash();
        nDefaultPort = 49444;

        assert(genesis.hashMerkleRoot == uint256S("0xf67057581d7f6b4316cd854aac32d14228f9d9ed92125317eeb2efbadc395d01"));
        assert(consensus.hashGenesisBlock == uint256S("0x7a6d81883f0f5984f861cbcc8b844de73757012938e39c3b7f29e9d4a050761f"));

        nPruneAfterHeight = 1000;

        vFixedSeeds.clear(); //! Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();  //! Regtest mode doesn't have any DNS seeds.

        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true;
        fTestnetToBeDeprecatedFieldRPC = false;

        checkpointData = (Checkpoints::CCheckpointData){
            boost::assign::map_list_of
            (0, consensus.hashGenesisBlock),
            1511954736,
            1,
            0
        };

        // Founders reward script expects a vector of 2-of-3 multisig addresses
        vFoundersRewardAddress = { };
        //assert(vFoundersRewardAddress.size() <= consensus.GetLastFoundersRewardBlockHeight());
    }
};
static CRegTestParams regTestParams;

static CChainParams *pCurrentParams = 0;

const CChainParams &Params() {
    assert(pCurrentParams);
    return *pCurrentParams;
}

CChainParams &Params(CBaseChainParams::Network network) {
    switch (network) {
        case CBaseChainParams::MAIN:
            return mainParams;
        case CBaseChainParams::TESTNET:
            return testNetParams;
        case CBaseChainParams::REGTEST:
            return regTestParams;
        default:
            assert(false && "Unimplemented network");
            return mainParams;
    }
}

void SelectParams(CBaseChainParams::Network network) {
    SelectBaseParams(network);
    pCurrentParams = &Params(network);

    // Some python qa rpc tests need to enforce the coinbase consensus rule
    if (network == CBaseChainParams::REGTEST && mapArgs.count("-regtestprotectcoinbase")) {
        regTestParams.SetRegTestCoinbaseMustBeProtected();
    }
}

bool SelectParamsFromCommandLine()
{
    CBaseChainParams::Network network = NetworkIdFromCommandLine();
    if (network == CBaseChainParams::MAX_NETWORK_TYPES)
        return false;

    SelectParams(network);
    return true;
}


// Block height must be >0 and <=last founders reward block height
// Index variable i ranges from 0 - (vFoundersRewardAddress.size()-1)
std::string CChainParams::GetFoundersRewardAddressAtHeight(int nHeight) const {
    int maxHeight = consensus.GetLastFoundersRewardBlockHeight();
    assert(nHeight > 0 && nHeight <= maxHeight);

    size_t addressChangeInterval = (maxHeight + vFoundersRewardAddress.size()) / vFoundersRewardAddress.size();
    size_t i = nHeight / addressChangeInterval;
    return vFoundersRewardAddress[i];
}

// Block height must be >0 and <=last founders reward block height
// The founders reward address is expected to be a multisig (P2SH) address
CScript CChainParams::GetFoundersRewardScriptAtHeight(int nHeight) const {
    assert(nHeight > 0 && nHeight <= consensus.GetLastFoundersRewardBlockHeight());

    CBitcoinAddress address(GetFoundersRewardAddressAtHeight(nHeight).c_str());
    assert(address.IsValid());
    assert(address.IsScript());
    CScriptID scriptID = get<CScriptID>(address.Get()); // Get() returns a boost variant
    CScript script = CScript() << OP_HASH160 << ToByteVector(scriptID) << OP_EQUAL;
    return script;
}

std::string CChainParams::GetFoundersRewardAddressAtIndex(int i) const {
    assert(i >= 0 && i < vFoundersRewardAddress.size());
    return vFoundersRewardAddress[i];
}

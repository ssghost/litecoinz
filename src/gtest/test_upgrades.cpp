#include <gtest/gtest.h>

#include <chainparams.h>
#include <consensus/upgrades.h>

#include <boost/optional.hpp>

class UpgradesTest : public ::testing::Test {
protected:
    virtual void SetUp() {
    }

    virtual void TearDown() {
        // Revert to default
        UpdateNetworkUpgradeParameters(Consensus::UPGRADE_TESTDUMMY, Consensus::NetworkUpgrade::NO_ACTIVATION_HEIGHT);
    }
};

TEST_F(UpgradesTest, NetworkUpgradeState) {
    const auto chainParams = CreateChainParams(CBaseChainParams::REGTEST);

    // Consensus::NetworkUpgrade::NO_ACTIVATION_HEIGHT
    EXPECT_EQ(
        NetworkUpgradeState(0, chainParams->GetConsensus(), Consensus::UPGRADE_TESTDUMMY),
        UPGRADE_DISABLED);
    EXPECT_EQ(
        NetworkUpgradeState(1000000, chainParams->GetConsensus(), Consensus::UPGRADE_TESTDUMMY),
        UPGRADE_DISABLED);

    UpdateNetworkUpgradeParameters(Consensus::UPGRADE_TESTDUMMY, Consensus::NetworkUpgrade::ALWAYS_ACTIVE);

    EXPECT_EQ(
        NetworkUpgradeState(0, chainParams->GetConsensus(), Consensus::UPGRADE_TESTDUMMY),
        UPGRADE_ACTIVE);
    EXPECT_EQ(
        NetworkUpgradeState(1000000, chainParams->GetConsensus(), Consensus::UPGRADE_TESTDUMMY),
        UPGRADE_ACTIVE);

    int nActivationHeight = 100;
    UpdateNetworkUpgradeParameters(Consensus::UPGRADE_TESTDUMMY, nActivationHeight);

    EXPECT_EQ(
        NetworkUpgradeState(0, chainParams->GetConsensus(), Consensus::UPGRADE_TESTDUMMY),
        UPGRADE_PENDING);
    EXPECT_EQ(
        NetworkUpgradeState(nActivationHeight - 1, chainParams->GetConsensus(), Consensus::UPGRADE_TESTDUMMY),
        UPGRADE_PENDING);
    EXPECT_EQ(
        NetworkUpgradeState(nActivationHeight, chainParams->GetConsensus(), Consensus::UPGRADE_TESTDUMMY),
        UPGRADE_ACTIVE);
    EXPECT_EQ(
        NetworkUpgradeState(1000000, chainParams->GetConsensus(), Consensus::UPGRADE_TESTDUMMY),
        UPGRADE_ACTIVE);
}

TEST_F(UpgradesTest, CurrentEpoch) {
    const auto chainParams = CreateChainParams(CBaseChainParams::MAIN);
    auto nBranchId = NetworkUpgradeInfo[Consensus::UPGRADE_TESTDUMMY].nBranchId;

    // Consensus::NetworkUpgrade::NO_ACTIVATION_HEIGHT
    EXPECT_EQ(CurrentEpoch(0, chainParams->GetConsensus()), Consensus::BASE_SPROUT);
    EXPECT_EQ(CurrentEpochBranchId(0, chainParams->GetConsensus()), 0);
    EXPECT_EQ(CurrentEpoch(1000000, chainParams->GetConsensus()), Consensus::BASE_SPROUT);
    EXPECT_EQ(CurrentEpochBranchId(1000000, chainParams->GetConsensus()), 0);

    UpdateNetworkUpgradeParameters(Consensus::UPGRADE_TESTDUMMY, Consensus::NetworkUpgrade::ALWAYS_ACTIVE);

    EXPECT_EQ(CurrentEpoch(0, chainParams->GetConsensus()), Consensus::UPGRADE_TESTDUMMY);
    EXPECT_EQ(CurrentEpochBranchId(0, chainParams->GetConsensus()), nBranchId);
    EXPECT_EQ(CurrentEpoch(1000000, chainParams->GetConsensus()), Consensus::UPGRADE_TESTDUMMY);
    EXPECT_EQ(CurrentEpochBranchId(1000000, chainParams->GetConsensus()), nBranchId);

    int nActivationHeight = 100;
    UpdateNetworkUpgradeParameters(Consensus::UPGRADE_TESTDUMMY, nActivationHeight);

    EXPECT_EQ(CurrentEpoch(0, chainParams->GetConsensus()), Consensus::BASE_SPROUT);
    EXPECT_EQ(CurrentEpochBranchId(0, chainParams->GetConsensus()), 0);
    EXPECT_EQ(CurrentEpoch(nActivationHeight - 1, chainParams->GetConsensus()), Consensus::BASE_SPROUT);
    EXPECT_EQ(CurrentEpochBranchId(nActivationHeight - 1, chainParams->GetConsensus()), 0);
    EXPECT_EQ(CurrentEpoch(nActivationHeight, chainParams->GetConsensus()), Consensus::UPGRADE_TESTDUMMY);
    EXPECT_EQ(CurrentEpochBranchId(nActivationHeight, chainParams->GetConsensus()), nBranchId);
    EXPECT_EQ(CurrentEpoch(1000000, chainParams->GetConsensus()), Consensus::UPGRADE_TESTDUMMY);
    EXPECT_EQ(CurrentEpochBranchId(1000000, chainParams->GetConsensus()), nBranchId);
}

TEST_F(UpgradesTest, IsActivationHeight) {
    const auto chainParams = CreateChainParams(CBaseChainParams::MAIN);

    // Consensus::NetworkUpgrade::NO_ACTIVATION_HEIGHT
    EXPECT_FALSE(IsActivationHeight(-1, chainParams->GetConsensus(), Consensus::UPGRADE_TESTDUMMY));
    EXPECT_FALSE(IsActivationHeight(0, chainParams->GetConsensus(), Consensus::UPGRADE_TESTDUMMY));
    EXPECT_FALSE(IsActivationHeight(1, chainParams->GetConsensus(), Consensus::UPGRADE_TESTDUMMY));
    EXPECT_FALSE(IsActivationHeight(1000000, chainParams->GetConsensus(), Consensus::UPGRADE_TESTDUMMY));

    UpdateNetworkUpgradeParameters(Consensus::UPGRADE_TESTDUMMY, Consensus::NetworkUpgrade::ALWAYS_ACTIVE);

    EXPECT_FALSE(IsActivationHeight(-1, chainParams->GetConsensus(), Consensus::UPGRADE_TESTDUMMY));
    EXPECT_TRUE(IsActivationHeight(0, chainParams->GetConsensus(), Consensus::UPGRADE_TESTDUMMY));
    EXPECT_FALSE(IsActivationHeight(1, chainParams->GetConsensus(), Consensus::UPGRADE_TESTDUMMY));
    EXPECT_FALSE(IsActivationHeight(1000000, chainParams->GetConsensus(), Consensus::UPGRADE_TESTDUMMY));

    int nActivationHeight = 100;
    UpdateNetworkUpgradeParameters(Consensus::UPGRADE_TESTDUMMY, nActivationHeight);

    EXPECT_FALSE(IsActivationHeight(-1, chainParams->GetConsensus(), Consensus::UPGRADE_TESTDUMMY));
    EXPECT_FALSE(IsActivationHeight(0, chainParams->GetConsensus(), Consensus::UPGRADE_TESTDUMMY));
    EXPECT_FALSE(IsActivationHeight(1, chainParams->GetConsensus(), Consensus::UPGRADE_TESTDUMMY));
    EXPECT_FALSE(IsActivationHeight(nActivationHeight - 1, chainParams->GetConsensus(), Consensus::UPGRADE_TESTDUMMY));
    EXPECT_TRUE(IsActivationHeight(nActivationHeight, chainParams->GetConsensus(), Consensus::UPGRADE_TESTDUMMY));
    EXPECT_FALSE(IsActivationHeight(nActivationHeight + 1, chainParams->GetConsensus(), Consensus::UPGRADE_TESTDUMMY));
    EXPECT_FALSE(IsActivationHeight(1000000, chainParams->GetConsensus(), Consensus::UPGRADE_TESTDUMMY));
}

TEST_F(UpgradesTest, IsActivationHeightForAnyUpgrade) {
    const auto chainParams = CreateChainParams(CBaseChainParams::MAIN);

    // Consensus::NetworkUpgrade::NO_ACTIVATION_HEIGHT
    EXPECT_FALSE(IsActivationHeightForAnyUpgrade(-1, chainParams->GetConsensus()));
    EXPECT_FALSE(IsActivationHeightForAnyUpgrade(0, chainParams->GetConsensus()));
    EXPECT_FALSE(IsActivationHeightForAnyUpgrade(1, chainParams->GetConsensus()));
    EXPECT_FALSE(IsActivationHeightForAnyUpgrade(1000000, chainParams->GetConsensus()));

    UpdateNetworkUpgradeParameters(Consensus::UPGRADE_TESTDUMMY, Consensus::NetworkUpgrade::ALWAYS_ACTIVE);

    EXPECT_FALSE(IsActivationHeightForAnyUpgrade(-1, chainParams->GetConsensus()));
    EXPECT_TRUE(IsActivationHeightForAnyUpgrade(0, chainParams->GetConsensus()));
    EXPECT_FALSE(IsActivationHeightForAnyUpgrade(1, chainParams->GetConsensus()));
    EXPECT_FALSE(IsActivationHeightForAnyUpgrade(1000000, chainParams->GetConsensus()));

    int nActivationHeight = 100;
    UpdateNetworkUpgradeParameters(Consensus::UPGRADE_TESTDUMMY, nActivationHeight);

    EXPECT_FALSE(IsActivationHeightForAnyUpgrade(-1, chainParams->GetConsensus()));
    EXPECT_FALSE(IsActivationHeightForAnyUpgrade(0, chainParams->GetConsensus()));
    EXPECT_FALSE(IsActivationHeightForAnyUpgrade(1, chainParams->GetConsensus()));
    EXPECT_FALSE(IsActivationHeightForAnyUpgrade(nActivationHeight - 1, chainParams->GetConsensus()));
    EXPECT_TRUE(IsActivationHeightForAnyUpgrade(nActivationHeight, chainParams->GetConsensus()));
    EXPECT_FALSE(IsActivationHeightForAnyUpgrade(nActivationHeight + 1, chainParams->GetConsensus()));
    EXPECT_FALSE(IsActivationHeightForAnyUpgrade(1000000, chainParams->GetConsensus()));
}

TEST_F(UpgradesTest, NextEpoch) {
    const auto chainParams = CreateChainParams(CBaseChainParams::MAIN);

    // Consensus::NetworkUpgrade::NO_ACTIVATION_HEIGHT
    EXPECT_EQ(NextEpoch(-1, chainParams->GetConsensus()), boost::none);
    EXPECT_EQ(NextEpoch(0, chainParams->GetConsensus()), boost::none);
    EXPECT_EQ(NextEpoch(1, chainParams->GetConsensus()), boost::none);
    EXPECT_EQ(NextEpoch(1000000, chainParams->GetConsensus()), boost::none);

    UpdateNetworkUpgradeParameters(Consensus::UPGRADE_TESTDUMMY, Consensus::NetworkUpgrade::ALWAYS_ACTIVE);

    EXPECT_EQ(NextEpoch(-1, chainParams->GetConsensus()), boost::none);
    EXPECT_EQ(NextEpoch(0, chainParams->GetConsensus()), boost::none);
    EXPECT_EQ(NextEpoch(1, chainParams->GetConsensus()), boost::none);
    EXPECT_EQ(NextEpoch(1000000, chainParams->GetConsensus()), boost::none);

    int nActivationHeight = 100;
    UpdateNetworkUpgradeParameters(Consensus::UPGRADE_TESTDUMMY, nActivationHeight);

    EXPECT_EQ(NextEpoch(-1, chainParams->GetConsensus()), boost::none);
    EXPECT_EQ(NextEpoch(0, chainParams->GetConsensus()), static_cast<int>(Consensus::UPGRADE_TESTDUMMY));
    EXPECT_EQ(NextEpoch(1, chainParams->GetConsensus()), static_cast<int>(Consensus::UPGRADE_TESTDUMMY));
    EXPECT_EQ(NextEpoch(nActivationHeight - 1, chainParams->GetConsensus()), static_cast<int>(Consensus::UPGRADE_TESTDUMMY));
    EXPECT_EQ(NextEpoch(nActivationHeight, chainParams->GetConsensus()), boost::none);
    EXPECT_EQ(NextEpoch(nActivationHeight + 1, chainParams->GetConsensus()), boost::none);
    EXPECT_EQ(NextEpoch(1000000, chainParams->GetConsensus()), boost::none);
}

TEST_F(UpgradesTest, NextActivationHeight) {
    const auto chainParams = CreateChainParams(CBaseChainParams::MAIN);

    // Consensus::NetworkUpgrade::NO_ACTIVATION_HEIGHT
    EXPECT_EQ(NextActivationHeight(-1, chainParams->GetConsensus()), boost::none);
    EXPECT_EQ(NextActivationHeight(0, chainParams->GetConsensus()), boost::none);
    EXPECT_EQ(NextActivationHeight(1, chainParams->GetConsensus()), boost::none);
    EXPECT_EQ(NextActivationHeight(1000000, chainParams->GetConsensus()), boost::none);

    UpdateNetworkUpgradeParameters(Consensus::UPGRADE_TESTDUMMY, Consensus::NetworkUpgrade::ALWAYS_ACTIVE);

    EXPECT_EQ(NextActivationHeight(-1, chainParams->GetConsensus()), boost::none);
    EXPECT_EQ(NextActivationHeight(0, chainParams->GetConsensus()), boost::none);
    EXPECT_EQ(NextActivationHeight(1, chainParams->GetConsensus()), boost::none);
    EXPECT_EQ(NextActivationHeight(1000000, chainParams->GetConsensus()), boost::none);

    int nActivationHeight = 100;
    UpdateNetworkUpgradeParameters(Consensus::UPGRADE_TESTDUMMY, nActivationHeight);

    EXPECT_EQ(NextActivationHeight(-1, chainParams->GetConsensus()), boost::none);
    EXPECT_EQ(NextActivationHeight(0, chainParams->GetConsensus()), nActivationHeight);
    EXPECT_EQ(NextActivationHeight(1, chainParams->GetConsensus()), nActivationHeight);
    EXPECT_EQ(NextActivationHeight(nActivationHeight - 1, chainParams->GetConsensus()), nActivationHeight);
    EXPECT_EQ(NextActivationHeight(nActivationHeight, chainParams->GetConsensus()), boost::none);
    EXPECT_EQ(NextActivationHeight(nActivationHeight + 1, chainParams->GetConsensus()), boost::none);
    EXPECT_EQ(NextActivationHeight(1000000, chainParams->GetConsensus()), boost::none);
}

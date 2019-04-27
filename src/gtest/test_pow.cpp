#include <gtest/gtest.h>

#include <chain.h>
#include <chainparams.h>
#include <pow.h>
#include <random.h>

TEST(PoW, DifficultyAveraging) {
    const auto chainParams = CreateChainParams(CBaseChainParams::MAIN);
    size_t lastBlk = 2 * chainParams->GetConsensus().nPowAveragingWindow;
    size_t firstBlk = lastBlk - chainParams->GetConsensus().nPowAveragingWindow;

    // Start with blocks evenly-spaced and equal difficulty
    std::vector<CBlockIndex> blocks(lastBlk+1);
    for (int i = 0; i <= lastBlk; i++) {
        blocks[i].pprev = i ? &blocks[i - 1] : nullptr;
        blocks[i].nHeight = i;
        blocks[i].nTime = 1269211443 + i * chainParams->GetConsensus().nPowTargetSpacing;
        blocks[i].nBits = 0x1e7fffff; /* target 0x007fffff000... */
        blocks[i].nChainWork = i ? blocks[i - 1].nChainWork + GetBlockProof(blocks[i - 1]) : arith_uint256(0);
    }

    // Result should be the same as if last difficulty was used
    arith_uint256 bnAvg;
    bnAvg.SetCompact(blocks[lastBlk].nBits);
    EXPECT_EQ(CalculateNextWorkRequired(bnAvg,
                                        blocks[lastBlk].GetMedianTimePast(),
                                        blocks[firstBlk].GetMedianTimePast(),
                                        chainParams->GetConsensus()),
              GetNextWorkRequired(&blocks[lastBlk], nullptr, chainParams->GetConsensus()));
    // Result should be unchanged, modulo integer division precision loss
    arith_uint256 bnRes;
    bnRes.SetCompact(0x1e7fffff);
    bnRes /= chainParams->GetConsensus().AveragingWindowTimespan();
    bnRes *= chainParams->GetConsensus().AveragingWindowTimespan();
    EXPECT_EQ(bnRes.GetCompact(), GetNextWorkRequired(&blocks[lastBlk], nullptr, chainParams->GetConsensus()));

    // Randomise the final block time (plus 1 to ensure it is always different)
    blocks[lastBlk].nTime += GetRand(chainParams->GetConsensus().nPowTargetSpacing/2) + 1;

    // Result should be the same as if last difficulty was used
    bnAvg.SetCompact(blocks[lastBlk].nBits);
    EXPECT_EQ(CalculateNextWorkRequired(bnAvg,
                                        blocks[lastBlk].GetMedianTimePast(),
                                        blocks[firstBlk].GetMedianTimePast(),
                                        chainParams->GetConsensus()),
              GetNextWorkRequired(&blocks[lastBlk], nullptr, chainParams->GetConsensus()));
    // Result should not be unchanged
    EXPECT_NE(0x1e7fffff, GetNextWorkRequired(&blocks[lastBlk], nullptr, chainParams->GetConsensus()));

    // Change the final block difficulty
    blocks[lastBlk].nBits = 0x1e0fffff;

    // Result should not be the same as if last difficulty was used
    bnAvg.SetCompact(blocks[lastBlk].nBits);
    EXPECT_NE(CalculateNextWorkRequired(bnAvg,
                                        blocks[lastBlk].GetMedianTimePast(),
                                        blocks[firstBlk].GetMedianTimePast(),
                                        chainParams->GetConsensus()),
              GetNextWorkRequired(&blocks[lastBlk], nullptr, chainParams->GetConsensus()));

    // Result should be the same as if the average difficulty was used
    arith_uint256 average = UintToArith256(uint256S("0000796968696969696969696969696969696969696969696969696969696969"));
    EXPECT_EQ(CalculateNextWorkRequired(average,
                                        blocks[lastBlk].GetMedianTimePast(),
                                        blocks[firstBlk].GetMedianTimePast(),
                                        chainParams->GetConsensus()),
              GetNextWorkRequired(&blocks[lastBlk], nullptr, chainParams->GetConsensus()));
}

TEST(PoW, MinDifficultyRules) {
    const auto chainParams = CreateChainParams(CBaseChainParams::TESTNET);
    size_t lastBlk = 2 * chainParams->GetConsensus().nPowAveragingWindow;
    size_t firstBlk = lastBlk - chainParams->GetConsensus().nPowAveragingWindow;

    // Start with blocks evenly-spaced and equal difficulty
    std::vector<CBlockIndex> blocks(lastBlk+1);
    for (int i = 0; i <= lastBlk; i++) {
        blocks[i].pprev = i ? &blocks[i - 1] : nullptr;
        blocks[i].nHeight = chainParams->GetConsensus().nPowAllowMinDifficultyBlocksAfterHeight.get() + i;
        blocks[i].nTime = 1269211443 + i * chainParams->GetConsensus().nPowTargetSpacing;
        blocks[i].nBits = 0x1e7fffff; /* target 0x007fffff000... */
        blocks[i].nChainWork = i ? blocks[i - 1].nChainWork + GetBlockProof(blocks[i - 1]) : arith_uint256(0);
    }

    // Create a new block at the target spacing
    CBlockHeader next;
    next.nTime = blocks[lastBlk].nTime + chainParams->GetConsensus().nPowTargetSpacing;

    // Result should be unchanged, modulo integer division precision loss
    arith_uint256 bnRes;
    bnRes.SetCompact(0x1e7fffff);
    bnRes /= chainParams->GetConsensus().AveragingWindowTimespan();
    bnRes *= chainParams->GetConsensus().AveragingWindowTimespan();
    EXPECT_EQ(GetNextWorkRequired(&blocks[lastBlk], &next, chainParams->GetConsensus()), bnRes.GetCompact());

    // Delay last block up to the edge of the min-difficulty limit
    next.nTime += chainParams->GetConsensus().nPowTargetSpacing * 5;

    // Result should be unchanged, modulo integer division precision loss
    EXPECT_EQ(GetNextWorkRequired(&blocks[lastBlk], &next, chainParams->GetConsensus()), bnRes.GetCompact());

    // Delay last block over the min-difficulty limit
    next.nTime += 1;

    // Result should be the minimum difficulty
    EXPECT_EQ(GetNextWorkRequired(&blocks[lastBlk], &next, chainParams->GetConsensus()),
              UintToArith256(chainParams->GetConsensus().powLimit).GetCompact());
}

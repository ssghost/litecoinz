#ifndef BITCOIN_TEST_TEST_BITCOIN_H
#define BITCOIN_TEST_TEST_BITCOIN_H

#include <consensus/upgrades.h>
#include <fs.h>
#include <pubkey.h>
#include <txdb.h>
#include <txmempool.h>

#include <memory>

#include <boost/thread.hpp>

/** Basic testing setup.
 * This just configures logging and chain parameters.
 */
struct BasicTestingSetup {
    ECCVerifyHandle globalVerifyHandle;

    BasicTestingSetup();
    ~BasicTestingSetup();
};

// Setup w.r.t. zk-SNARK API
struct JoinSplitTestingSetup: public BasicTestingSetup {
    JoinSplitTestingSetup();
    ~JoinSplitTestingSetup();
};

/** Testing setup that configures a complete environment.
 * Included are data directory, coins database, script check threads
 * and wallet (if enabled) setup.
 */
class CConnman;
struct TestingSetup: public JoinSplitTestingSetup {
    CCoinsViewDB *pcoinsdbview;
    fs::path orig_current_path;
    fs::path pathTemp;
    boost::thread_group threadGroup;
    CConnman* connman;

    TestingSetup();
    ~TestingSetup();
};

class CTxMemPoolEntry;
class CTxMemPool;

struct TestMemPoolEntryHelper
{
    // Default values
    CAmount nFee;
    int64_t nTime;
    double dPriority;
    unsigned int nHeight;
    bool hadNoDependencies;
    bool spendsCoinbase;
    uint32_t nBranchId;

    unsigned int sigOpCount;

    LockPoints lp;

    TestMemPoolEntryHelper() :
        nFee(0), nTime(0), dPriority(0.0), nHeight(1),
        hadNoDependencies(false), spendsCoinbase(false), sigOpCount(1),
        nBranchId(SPROUT_BRANCH_ID) { }

    CTxMemPoolEntry FromTx(CMutableTransaction &tx, CTxMemPool *pool = nullptr);

    // Change the default value
    TestMemPoolEntryHelper &Fee(CAmount _fee) { nFee = _fee; return *this; }
    TestMemPoolEntryHelper &Time(int64_t _time) { nTime = _time; return *this; }
    TestMemPoolEntryHelper &Priority(double _priority) { dPriority = _priority; return *this; }
    TestMemPoolEntryHelper &Height(unsigned int _height) { nHeight = _height; return *this; }
    TestMemPoolEntryHelper &HadNoDependencies(bool _hnd) { hadNoDependencies = _hnd; return *this; }
    TestMemPoolEntryHelper &SpendsCoinbase(bool _flag) { spendsCoinbase = _flag; return *this; }
    TestMemPoolEntryHelper &SigOps(unsigned int _sigops) { sigOpCount = _sigops; return *this; }
    TestMemPoolEntryHelper &BranchId(uint32_t _branchId) { nBranchId = _branchId; return *this; }
};
#endif

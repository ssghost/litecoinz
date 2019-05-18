// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_CONSENSUS_CONSENSUS_H
#define BITCOIN_CONSENSUS_CONSENSUS_H

#include <stdint.h>

/** The minimum allowed block version (network rule) */
static const int32_t MIN_BLOCK_VERSION = 4;
/** The minimum allowed transaction version (network rule) */
static const int32_t SPROUT_MIN_TX_VERSION = 1;
/** The minimum allowed Overwinter transaction version (network rule) */
static const int32_t OVERWINTER_MIN_TX_VERSION = 3;
/** The maximum allowed Overwinter transaction version (network rule) */
static const int32_t OVERWINTER_MAX_TX_VERSION = 3;
/** The minimum allowed Sapling transaction version (network rule) */
static const int32_t SAPLING_MIN_TX_VERSION = 4;
/** The maximum allowed Sapling transaction version (network rule) */
static const int32_t SAPLING_MAX_TX_VERSION = 4;
/** The maximum allowed cost for a block, see BIP 141 (network rule) */
static const unsigned int MAX_BLOCK_COST = 4000000;
/** The maximum allowed size for a serialized block, in bytes (network rule) */
static const unsigned int MAX_BLOCK_SIZE = 2000000;
/** The maximum allowed number of signature check operations in a block (network rule) */
static const unsigned int MAX_BLOCK_SIGOPS = 20000;
/** The maximum size of a transaction (network rule) */
static const unsigned int MAX_TX_SIZE_BEFORE_SAPLING = 100000;
static const unsigned int MAX_TX_SIZE_AFTER_SAPLING = 400000;
/** Coinbase transaction outputs can only be spent after this number of new blocks (network rule) */
static const int COINBASE_MATURITY = 100;
/** The minimum value which is invalid for expiry height, used by CTransaction and CMutableTransaction */
static constexpr uint32_t TX_EXPIRY_HEIGHT_THRESHOLD = 500000000;

/** Flags for nSequence and nLockTime locks */
enum {
    /* Interpret sequence numbers as relative lock-time constraints. */
    LOCKTIME_VERIFY_SEQUENCE = (1 << 0),

    /* Use GetMedianTimePast() instead of nTime for end point timestamp. */
    LOCKTIME_MEDIAN_TIME_PAST = (1 << 1),
};

#endif // BITCOIN_CONSENSUS_CONSENSUS_H

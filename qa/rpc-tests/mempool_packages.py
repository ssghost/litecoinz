#!/usr/bin/env python2
# Copyright (c) 2014-2015 The Bitcoin Core developers
# Distributed under the MIT software license, see the accompanying
# file COPYING or http://www.opensource.org/licenses/mit-license.php.

# Test descendant package tracking code

from test_framework.test_framework import BitcoinTestFramework
from test_framework.util import *

def satoshi_round(amount):
    return  Decimal(amount).quantize(Decimal('0.00000001'), rounding=ROUND_DOWN)

MAX_ANCESTORS = 25
MAX_DESCENDANTS = 25

class MempoolPackagesTest(BitcoinTestFramework):

    def setup_network(self):
        self.nodes = []
        self.nodes.append(start_node(0, self.options.tmpdir, ["-maxorphantx=1000", "-relaypriority=0"]))
        self.is_network_split = False
        self.sync_all()

    # Build a transaction that spends parent_txid:vout
    # Return amount sent
    def chain_transaction(self, parent_txid, vout, value, fee, num_outputs):
        send_value = satoshi_round((value - fee)/num_outputs)
        inputs = [ {'txid' : parent_txid, 'vout' : vout} ]
        outputs = {}
        for i in xrange(num_outputs):
            outputs[self.nodes[0].getnewaddress()] = send_value
        rawtx = self.nodes[0].createrawtransaction(inputs, outputs)
        signedtx = self.nodes[0].signrawtransaction(rawtx)
        txid = self.nodes[0].sendrawtransaction(signedtx['hex'])
        fulltx = self.nodes[0].getrawtransaction(txid, 1)
        assert(len(fulltx['vout']) == num_outputs) # make sure we didn't generate a change output
        return (txid, send_value)

    def run_test(self):
        ''' Mine some blocks and have them mature. '''
        self.nodes[0].generate(101)
        utxo = self.nodes[0].listunspent(10)
        txid = utxo[0]['txid']
        vout = utxo[0]['vout']
        value = utxo[0]['amount']

        fee = Decimal("0.0001")
        # MAX_ANCESTORS transactions off a confirmed tx should be fine
        chain = []
        for i in xrange(MAX_ANCESTORS):
            (txid, sent_value) = self.chain_transaction(txid, 0, value, fee, 1)
            value = sent_value
            chain.append(txid)

        # Check mempool has MAX_ANCESTORS transactions in it, and descendant
        # count and fees should look correct
        mempool = self.nodes[0].getrawmempool(True)
        assert_equal(len(mempool), MAX_ANCESTORS)
        descendant_count = 1
        descendant_fees = 0
        descendant_size = 0
        SATOSHIS = 100000000

        for x in reversed(chain):
            assert_equal(mempool[x]['descendantcount'], descendant_count)
            descendant_fees += mempool[x]['fee']
            assert_equal(mempool[x]['modifiedfee'], mempool[x]['fee'])
            assert_equal(mempool[x]['descendantfees'], SATOSHIS*descendant_fees)
            descendant_size += mempool[x]['size']
            assert_equal(mempool[x]['descendantsize'], descendant_size)
            descendant_count += 1

        # Check that descendant modified fees includes fee deltas from
        # prioritisetransaction
        self.nodes[0].prioritisetransaction(chain[-1], 0, 1000)
        mempool = self.nodes[0].getrawmempool(True)

        descendant_fees = 0
        for x in reversed(chain):
            descendant_fees += mempool[x]['fee']
            assert_equal(mempool[x]['descendantfees'], SATOSHIS*descendant_fees+1000)

        # Adding one more transaction on to the chain should fail.
        try:
            self.chain_transaction(txid, vout, value, fee, 1)
        except JSONRPCException as e:
            print "too-long-ancestor-chain successfully rejected"

        # Check that prioritising a tx before it's added to the mempool works
        self.nodes[0].generate(1)
        self.nodes[0].prioritisetransaction(chain[-1], 0, 2000)
        self.nodes[0].invalidateblock(self.nodes[0].getbestblockhash())
        mempool = self.nodes[0].getrawmempool(True)

        descendant_fees = 0
        for x in reversed(chain):
            descendant_fees += mempool[x]['fee']
            if (x == chain[-1]):
                assert_equal(mempool[x]['modifiedfee'], mempool[x]['fee']+satoshi_round(0.00002))
            assert_equal(mempool[x]['descendantfees'], SATOSHIS*descendant_fees+2000)

        # TODO: test ancestor size limits

        # Now test descendant chain limits
        txid = utxo[1]['txid']
        value = utxo[1]['amount']
        vout = utxo[1]['vout']

        transaction_package = []
        # First create one parent tx with 10 children
        (txid, sent_value) = self.chain_transaction(txid, vout, value, fee, 10)
        parent_transaction = txid
        for i in xrange(10):
            transaction_package.append({'txid': txid, 'vout': i, 'amount': sent_value})

        for i in xrange(MAX_DESCENDANTS):
            utxo = transaction_package.pop(0)
            try:
                (txid, sent_value) = self.chain_transaction(utxo['txid'], utxo['vout'], utxo['amount'], fee, 10)
                for j in xrange(10):
                    transaction_package.append({'txid': txid, 'vout': j, 'amount': sent_value})
                if i == MAX_DESCENDANTS - 2:
                    mempool = self.nodes[0].getrawmempool(True)
                    assert_equal(mempool[parent_transaction]['descendantcount'], MAX_DESCENDANTS)
            except JSONRPCException as e:
                print e.error['message']
                assert_equal(i, MAX_DESCENDANTS - 1)
                print "tx that would create too large descendant package successfully rejected"

        # TODO: test descendant size limits

if __name__ == '__main__':
    MempoolPackagesTest().main()

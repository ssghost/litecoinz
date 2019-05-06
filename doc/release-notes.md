Negative confirmations and conflict detection
---------------------------------------------

The wallet will now report a negative number for confirmations that indicates
how deep in the block chain the conflict is found. For example, if a transaction
A has 5 confirmations and spends the same input as a wallet transaction B, B
will be reported as having -5 confirmations. If another wallet transaction C
spends an output from B, it will also be reported as having -5 confirmations.
To detect conflicts with historical transactions in the chain a one-time
`-rescan` may be needed.

Unlike earlier versions, unconfirmed but non-conflicting transactions will never
get a negative confirmation count. They are not treated as spendable unless
they're coming from ourself (change) and accepted into our local mempool,
however. The new "trusted" field in the `listtransactions` RPC output
indicates whether outputs of an unconfirmed transaction are considered
spendable.

Merkle branches removed from wallet
-----------------------------------
Previously, every wallet transaction stored a Merkle branch to prove its
presence in blocks. This wasn't being used for more than an expensive
sanity check. Since 2.0.2, these are no longer stored. When loading a
2.0.2 wallet into an older version, it will automatically rescan to avoid
failed checks.

Asm script outputs replacements for OP_NOP2 and OP_NOP3
-------------------------------------------------------

OP_NOP2 has been renamed to OP_CHECKLOCKTIMEVERIFY by [BIP 
65](https://github.com/bitcoin/bips/blob/master/bip-0065.mediawiki)

OP_NOP3 has been renamed to OP_CHECKSEQUENCEVERIFY by [BIP 
112](https://github.com/bitcoin/bips/blob/master/bip-0112.mediawiki)

The following outputs are affected by this change:
- RPC `getrawtransaction` (in verbose mode)
- RPC `decoderawtransaction`
- RPC `decodescript`
- REST `/rest/tx/` (JSON format)
- REST `/rest/block/` (JSON format when including extended tx details)
- `litecoinz-tx -json`

### RPC

The first boolean argument to `getaddednodeinfo` has been removed. This is an incompatible change.

### Logging

- The log timestamp format is now ISO 8601 (e.g. "2018-02-28T12:34:56Z").

### ZMQ

- The new RPC `getzmqnotifications` returns information about active ZMQ
  notifications.

### P2P protocol and network code

The p2p alert system has been removed in #7692 and the 'alert' message is no longer supported.

Fee filtering of invs (BIP 133)
------------------------------------

The optional new p2p message "feefilter" is implemented and the protocol
version is bumped to 70013. Upon receiving a feefilter message from a peer,
a node will not send invs for any transactions which do not meet the filter
feerate. [BIP 133](https://github.com/bitcoin/bips/blob/master/bip-0133.mediawiki)

New mempool information RPC calls
---------------------------------

RPC calls have been added to output detailed statistics for individual mempool
entries, as well as to calculate the in-mempool ancestors or descendants of a
transaction: see `getmempoolentry`, `getmempoolancestors`, `getmempooldescendants`.

Sprout to Sapling Migration Tool
--------------------------------
This release includes the addition of a tool that will enable users to migrate
shielded funds from the Sprout pool to the Sapling pool while minimizing
information leakage. 

The migration can be enabled using the RPC `z_setmigration` or by including
`-migration` in the `litecoinz.conf` file. Unless otherwise specified funds will be
migrated to the wallet's default Sapling address; it is also possible to set the 
receiving Sapling address using the `-migrationdestaddress` option in `litecoinz.conf`.

See [ZIP308](https://github.com/zcash/zips/blob/master/zip-0308.rst) for full details. 

64-bit ARMv8 support
--------------------
Added ARMv8 (AArch64) support. This enables users to build LitecoinZ on even more
devices.

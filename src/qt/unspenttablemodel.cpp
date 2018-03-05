// Copyright (c) 2017-2018 The LitecoinZ developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "unspenttablemodel.h"

#include "bitcoinunits.h"
#include "guiutil.h"
#include "walletmodel.h"
#include "optionsmodel.h"

#include "base58.h"
#include "wallet/wallet.h"
#include "main.h"

#include <boost/foreach.hpp>

#include <QFont>
#include <QDebug>

const QString UnspentTableModel::ZUnspent = "Z";
const QString UnspentTableModel::TUnspent = "T";

// Amount column is right-aligned it contains numbers
static int column_alignments[] = {
        Qt::AlignLeft|Qt::AlignVCenter, /* address */
        Qt::AlignLeft|Qt::AlignVCenter, /* txid */
        Qt::AlignRight|Qt::AlignVCenter /* amount */
    };

struct UnspentTableEntry
{
    enum Type {
        ZUnspent,
        TUnspent,
        Hidden
    };

    Type type;
    QString address;
    QString txid;
    CAmount amount;

    UnspentTableEntry() {}
    UnspentTableEntry(Type type, const QString &address, const QString &txid, const CAmount &amount):
        type(type), address(address), txid(txid), amount(amount) {}
};

struct UnspentTableEntryLessThan
{
    bool operator()(const UnspentTableEntry &a, const UnspentTableEntry &b) const
    {
        return a.address < b.address;
    }
    bool operator()(const UnspentTableEntry &a, const QString &b) const
    {
        return a.address < b;
    }
    bool operator()(const QString &a, const UnspentTableEntry &b) const
    {
        return a < b.address;
    }
};

/* Determine unspent type from unspent purpose */
static UnspentTableEntry::Type translateUnspentType(const QString &strPurpose)
{
    UnspentTableEntry::Type unspentType = UnspentTableEntry::Hidden;
    if (strPurpose == "tunspent")
        unspentType = UnspentTableEntry::TUnspent;
    else if (strPurpose == "zunspent")
        unspentType = UnspentTableEntry::ZUnspent;
    return unspentType;
}

// Private implementation
class UnspentTablePriv
{
public:
    CWallet *wallet;
    QList<UnspentTableEntry> cachedUnspentTable;
    UnspentTableModel *parent;

    UnspentTablePriv(CWallet *wallet, UnspentTableModel *parent):
        wallet(wallet), parent(parent) {}

    void refreshUnspentTable()
    {
        cachedUnspentTable.clear();
        {
            // T-Unspent
            std::vector<COutput> vecOutputs;

            LOCK2(cs_main, wallet->cs_wallet);
            wallet->AvailableCoins(vecOutputs, false, NULL, true);

            BOOST_FOREACH(const COutput& out, vecOutputs) {
                if (out.tx->IsCoinBase())
                    continue;

                if (!out.fSpendable)
                    continue;

                CTxDestination address;
                if (ExtractDestination(out.tx->vout[out.i].scriptPubKey, address)) {
                    UnspentTableEntry::Type unspentType = translateUnspentType(QString::fromStdString("tunspent"));
                    cachedUnspentTable.append(UnspentTableEntry(unspentType,
                                                  QString::fromStdString(CBitcoinAddress(address).ToString()),
                                                  QString::fromStdString(out.tx->GetHash().GetHex()),
                                                  CAmount(out.tx->vout[out.i].nValue)
                                             )
                    );
                }
            }

            // Z-Unspent
            std::set<libzcash::PaymentAddress> zaddrs = {};
            int nMinDepth = 1;
            int nMaxDepth = 9999999;

            std::set<libzcash::PaymentAddress> addresses;
            wallet->GetPaymentAddresses(addresses);
            for (auto addr : addresses ) {
                if (wallet->HaveSpendingKey(addr)) {
                    zaddrs.insert(addr);
                }
            }

            if (zaddrs.size() > 0) {
                std::vector<CUnspentNotePlaintextEntry> entries; 
                wallet->GetUnspentFilteredNotes(entries, zaddrs, nMinDepth, nMaxDepth);
                for (CUnspentNotePlaintextEntry & entry : entries) {
                    UnspentTableEntry::Type unspentType = translateUnspentType(QString::fromStdString("zunspent"));
                    cachedUnspentTable.append(UnspentTableEntry(unspentType,
                                                  QString::fromStdString(CZCPaymentAddress(entry.address).ToString()),
                                                  QString::fromStdString(entry.jsop.hash.ToString()),
                                                  CAmount(entry.plaintext.value)
                                             )
                    );
                }
            }

        }
        // qLowerBound() and qUpperBound() require our cachedUnspentTable list to be sorted in asc order
        // Even though the map is already sorted this re-sorting step is needed because the originating map
        // is sorted by binary address, not by base58() address.
        qSort(cachedUnspentTable.begin(), cachedUnspentTable.end(), UnspentTableEntryLessThan());
    }

    int size()
    {
        return cachedUnspentTable.size();
    }

    UnspentTableEntry *index(int idx)
    {
        if(idx >= 0 && idx < cachedUnspentTable.size())
        {
            return &cachedUnspentTable[idx];
        }
        else
        {
            return 0;
        }
    }
};

UnspentTableModel::UnspentTableModel(CWallet *wallet, WalletModel *parent) :
    QAbstractTableModel(parent), walletModel(parent), wallet(wallet), priv(0)
{
    columns << tr("Address") << tr("Txid") << BitcoinUnits::getAmountColumnTitle(walletModel->getOptionsModel()->getDisplayUnit());
    priv = new UnspentTablePriv(wallet, this);
    priv->refreshUnspentTable();

    connect(walletModel->getOptionsModel(), SIGNAL(displayUnitChanged(int)), this, SLOT(updateDisplayUnit()));
}

void UnspentTableModel::updateDisplayUnit()
{
    // emit dataChanged to update Amount column with the current unit
    updateAmountColumnTitle();
    Q_EMIT dataChanged(index(0, Amount), index(priv->size()-1, Amount));
}

UnspentTableModel::~UnspentTableModel()
{
    delete priv;
}

int UnspentTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return priv->size();
}

int UnspentTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return columns.length();
}

QVariant UnspentTableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    UnspentTableEntry *rec = static_cast<UnspentTableEntry*>(index.internalPointer());

    if(role == Qt::DisplayRole || role == Qt::EditRole)
    {
        switch(index.column())
        {
        case Address:
            return rec->address;
        case Txid:
            return rec->txid;
        case Amount:
            return BitcoinUnits::format(walletModel->getOptionsModel()->getDisplayUnit(), rec->amount);
        }
    }
    else if (role == Qt::FontRole)
    {
        QFont font;
        if(index.column() == Address)
        {
            font = GUIUtil::fixedPitchFont();
        }
        return font;
    }
    else if (role == TypeRole)
    {
        switch(rec->type)
        {
        case UnspentTableEntry::ZUnspent:
            return ZUnspent;
        case UnspentTableEntry::TUnspent:
            return TUnspent;
        default: break;
        }
    }
    else if (role == Qt::TextAlignmentRole)
    {
        return column_alignments[index.column()];
    }
    return QVariant();
}

QVariant UnspentTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal)
    {
        if(role == Qt::DisplayRole && section < columns.size())
        {
            return columns[section];
        }
        else if (role == Qt::TextAlignmentRole)
        {
            return column_alignments[section];
        }
    }
    return QVariant();
}

Qt::ItemFlags UnspentTableModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())
        return 0;
    UnspentTableEntry *rec = static_cast<UnspentTableEntry*>(index.internalPointer());

    Qt::ItemFlags retval = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    return retval;
}

QModelIndex UnspentTableModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    UnspentTableEntry *data = priv->index(row);
    if(data)
    {
        return createIndex(row, column, priv->index(row));
    }
    return QModelIndex();
}

int UnspentTableModel::lookupAddress(const QString &address) const
{
    QModelIndexList lst = match(index(0, Address, QModelIndex()),
                                Qt::EditRole, address, 1, Qt::MatchExactly);
    if(lst.isEmpty())
    {
        return -1;
    }
    else
    {
        return lst.at(0).row();
    }
}

void UnspentTableModel::emitDataChanged(int idx)
{
    Q_EMIT dataChanged(index(idx, 0, QModelIndex()), index(idx, columns.length()-1, QModelIndex()));
}

void UnspentTableModel::refresh()
{
    Q_EMIT layoutAboutToBeChanged();
    priv->refreshUnspentTable();
    Q_EMIT layoutChanged();
}

/** Updates the column title to "Amount (DisplayUnit)" and emits headerDataChanged() signal for table headers to react. */
void UnspentTableModel::updateAmountColumnTitle()
{
    columns[Amount] = BitcoinUnits::getAmountColumnTitle(walletModel->getOptionsModel()->getDisplayUnit());
    Q_EMIT headerDataChanged(Qt::Horizontal,Amount,Amount);
}


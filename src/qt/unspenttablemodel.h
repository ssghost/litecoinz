// Copyright (c) 2017-2018 The LitecoinZ developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_QT_UNSPENTTABLEMODEL_H
#define BITCOIN_QT_UNSPENTTABLEMODEL_H

#include <QAbstractTableModel>
#include <QStringList>

class UnspentTablePriv;
class WalletModel;

class CWallet;

/**
   Qt model of the unspent transaction.
 */
class UnspentTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit UnspentTableModel(CWallet *wallet, WalletModel *parent = 0);
    ~UnspentTableModel();

    enum ColumnIndex {
        Address = 0,
        Txid = 1,
        Amount = 2
    };

    enum RoleIndex {
        TypeRole = Qt::UserRole
    };

    static const QString ZUnspent;
    static const QString TUnspent;

    /** @name Methods overridden from QAbstractTableModel
        @{*/
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    /*@}*/

    /* Look up row index of an address in the model.
       Return -1 if not found.
     */
    int lookupAddress(const QString &address) const;

private:
    WalletModel *walletModel;
    CWallet *wallet;
    UnspentTablePriv *priv;
    QStringList columns;

    /** Notify listeners that data changed. */
    void emitDataChanged(int index);

public Q_SLOTS:
    void updateDisplayUnit();
    void refresh();

    /** Updates the column title to "Amount (DisplayUnit)" and emits headerDataChanged() signal for table headers to react. */
    void updateAmountColumnTitle();

    friend class UnspentTablePriv;
};

#endif // BITCOIN_QT_UNSPENTTABLEMODEL_H

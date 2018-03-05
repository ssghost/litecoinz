// Copyright (c) 2017-2018 The LitecoinZ developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_QT_COINSELECTIONTABLEMODEL_H
#define BITCOIN_QT_COINSELECTIONTABLEMODEL_H

#include <QAbstractTableModel>
#include <QStringList>

class CoinSelectionTablePriv;
class WalletModel;

class CWallet;

/**
   Qt model of the coinselection transaction.
 */
class CoinSelectionTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit CoinSelectionTableModel(CWallet *wallet, WalletModel *parent = 0);
    ~CoinSelectionTableModel();

    enum ColumnIndex {
        Address = 0,
        Amount = 1
    };

    enum RoleIndex {
        TypeRole = Qt::UserRole
    };

    static const QString ZCoinSelection;
    static const QString TCoinSelection;

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
    CoinSelectionTablePriv *priv;
    QStringList columns;

    /** Notify listeners that data changed. */
    void emitDataChanged(int index);

public Q_SLOTS:
    void updateDisplayUnit();
    void refresh();

    friend class CoinSelectionTablePriv;
};

#endif // BITCOIN_QT_COINSELECTIONTABLEMODEL_H

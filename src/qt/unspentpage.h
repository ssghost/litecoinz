// Copyright (c) 2017-2018 The LitecoinZ developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_QT_UNSPENTPAGE_H
#define BITCOIN_QT_UNSPENTPAGE_H

#include "walletmodel.h"

#include <QDialog>

class UnspentTableModel;
class OptionsModel;
class PlatformStyle;

namespace Ui {
    class UnspentPage;
}

QT_BEGIN_NAMESPACE
class QItemSelection;
class QMenu;
class QModelIndex;
class QSortFilterProxyModel;
class QTableView;
QT_END_NAMESPACE

/** Dialog for unspent transaction list */
class UnspentPage : public QDialog
{
    Q_OBJECT

public:
    explicit UnspentPage(const PlatformStyle *platformStyle,  QWidget *parent = 0);
    ~UnspentPage();

    void setModel(WalletModel *model);

private:
    Ui::UnspentPage *ui;
    WalletModel *model;
    UnspentTableModel *unspentModel;
    QSortFilterProxyModel *proxyModelUnspentZ;
    QSortFilterProxyModel *proxyModelUnspentT;
    QMenu *contextZMenu;
    QMenu *contextTMenu;

    const PlatformStyle *platformStyle;

private Q_SLOTS:
    void selectionUnspentZChanged();
    void selectionUnspentTChanged();
    void onCopyZAddressAction();
    void onCopyZTxidAction();
    void onCopyZAmountAction();
    void onCopyTAddressAction();
    void onCopyTTxidAction();
    void onCopyTAmountAction();
    void contextualZMenu(const QPoint &point);
    void contextualTMenu(const QPoint &point);

public Q_SLOTS:
    /** Refresh table model */
    void refreshTableModel();

};

#endif // BITCOIN_QT_UNSPENTPAGE_H

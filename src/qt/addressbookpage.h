// Copyright (c) 2017-2018 The LitecoinZ developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_QT_ADDRESSBOOKPAGE_H
#define BITCOIN_QT_ADDRESSBOOKPAGE_H

#include <QDialog>

class AddressTableModel;
class OptionsModel;
class PlatformStyle;

namespace Ui {
    class AddressBookPage;
}

QT_BEGIN_NAMESPACE
class QItemSelection;
class QMenu;
class QModelIndex;
class QSortFilterProxyModel;
class QTableView;
QT_END_NAMESPACE

/** Dialog for LitecoinZ Address Book */
class AddressBookPage : public QDialog
{
    Q_OBJECT

public:
    explicit AddressBookPage(const PlatformStyle *platformStyle, QWidget *parent = 0);
    ~AddressBookPage();

    void setModel(AddressTableModel *model);

private:
    Ui::AddressBookPage *ui;
    AddressTableModel *model;
    QSortFilterProxyModel *proxyModelReceivingZ;
    QSortFilterProxyModel *proxyModelReceivingT;
    QSortFilterProxyModel *proxyModelSendingT;

    QMenu *contextReceivingZMenu;
    QMenu *contextReceivingTMenu;
    QMenu *contextSendingTMenu;

    QAction *deleteSendingTAction; // to be able to explicitly disable it

    QString newReceivingZAddressToSelect;
    QString newReceivingTAddressToSelect;
    QString newSendingTAddressToSelect;

    const PlatformStyle *platformStyle;

private Q_SLOTS:
    /** Delete currently selected address entry */
    void on_deleteSendingTAddress_clicked();

    /** Create a new address for receiving coins and / or add a new address book entry */
    void on_newReceivingZAddress_clicked();
    void on_newReceivingTAddress_clicked();
    void on_newSendingTAddress_clicked();

    /** Copy address of currently selected address entry to clipboard */
    void on_copyReceivingZAddress_clicked();
    void on_copyReceivingTAddress_clicked();
    void on_copySendingTAddress_clicked();

    /** Export button clicked */
    void on_exportReceivingZAddress_clicked();
    void on_exportReceivingTAddress_clicked();
    void on_exportSendingTAddress_clicked();

    /** Set button states based on selection */
    void selectionReceivingZChanged();
    void selectionReceivingTChanged();
    void selectionSendingTChanged();

    /** Spawn contextual menu (right mouse menu) for address book entry */
    void contextualReceivingZMenu(const QPoint &point);
    void contextualReceivingTMenu(const QPoint &point);
    void contextualSendingTMenu(const QPoint &point);

    /** Copy label of currently selected address entry to clipboard (no button) */
    void onCopyLabelReceivingZAction();
    void onCopyLabelReceivingTAction();
    void onCopyLabelSendingTAction();

    /** Edit currently selected address entry (no button) */
    void onEditReceivingTAction();
    void onEditSendingTAction();

    /** New entry/entries were added to address table */
    void selectNewAddress(const QModelIndex &parent, int begin, int /*end*/);
};

#endif // BITCOIN_QT_ADDRESSBOOKPAGE_H

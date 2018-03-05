// Copyright (c) 2017-2018 The LitecoinZ developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#if defined(HAVE_CONFIG_H)
#include "config/bitcoin-config.h"
#endif

#include "coinselectiondialog.h"
#include "ui_coinselectiondialog.h"

#include "coinselectiontablemodel.h"
#include "bitcoingui.h"
#include "guiutil.h"
#include "platformstyle.h"
#include "walletmodel.h"

#include <QMenu>
#include <QSortFilterProxyModel>

CoinSelectionDialog::CoinSelectionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CoinSelectionDialog),
    model(0)
{
    ui->setupUi(this);

    ui->tableViewZ->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableViewT->setEditTriggers(QAbstractItemView::NoEditTriggers);

    setWindowTitle(tr("CoinSelection transactions list"));

    // Context menu actions
    QAction *copyZAddressAction = new QAction(tr("&Copy Address"), this);
    QAction *copyZAmountAction = new QAction(tr("Copy &Amount"), this);

    QAction *copyTAddressAction = new QAction(tr("&Copy Address"), this);
    QAction *copyTAmountAction = new QAction(tr("Copy &Amount"), this);

    // Build context menu
    contextZMenu = new QMenu(this);
    contextZMenu->addAction(copyZAddressAction);
    contextZMenu->addAction(copyZAmountAction);
    contextZMenu->addSeparator();

    contextTMenu = new QMenu(this);
    contextTMenu->addAction(copyTAddressAction);
    contextTMenu->addAction(copyTAmountAction);
    contextTMenu->addSeparator();

    // Connect signals for context menu actions
    connect(copyZAddressAction, SIGNAL(triggered()), this, SLOT(onCopyZAddressAction()));
    connect(copyZAmountAction, SIGNAL(triggered()), this, SLOT(onCopyZAmountAction()));

    connect(copyTAddressAction, SIGNAL(triggered()), this, SLOT(onCopyTAddressAction()));
    connect(copyTAmountAction, SIGNAL(triggered()), this, SLOT(onCopyTAmountAction()));

    connect(ui->tableViewZ, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextualZMenu(QPoint)));
    connect(ui->tableViewT, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextualTMenu(QPoint)));

    connect(ui->tableViewZ, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(accept()));
    connect(ui->tableViewT, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(accept()));
    connect(ui->closeButton, SIGNAL(clicked()), this, SLOT(accept()));
}

CoinSelectionDialog::~CoinSelectionDialog()
{
    delete ui;
}

void CoinSelectionDialog::onCopyZAddressAction()
{
    GUIUtil::copyEntryData(ui->tableViewZ, CoinSelectionTableModel::Address);
}

void CoinSelectionDialog::onCopyZAmountAction()
{
    GUIUtil::copyEntryData(ui->tableViewZ, CoinSelectionTableModel::Amount);
}

void CoinSelectionDialog::onCopyTAddressAction()
{
    GUIUtil::copyEntryData(ui->tableViewT, CoinSelectionTableModel::Address);
}

void CoinSelectionDialog::onCopyTAmountAction()
{
    GUIUtil::copyEntryData(ui->tableViewT, CoinSelectionTableModel::Amount);
}

void CoinSelectionDialog::contextualZMenu(const QPoint &point)
{
    QModelIndex index = ui->tableViewZ->indexAt(point);
    if(index.isValid())
    {
        contextZMenu->exec(QCursor::pos());
    }
}

void CoinSelectionDialog::contextualTMenu(const QPoint &point)
{
    QModelIndex index = ui->tableViewT->indexAt(point);
    if(index.isValid())
    {
        contextTMenu->exec(QCursor::pos());
    }
}

void CoinSelectionDialog::setModel(WalletModel *model)
{
    this->model = model;
    if(!model)
        return;

    coinSelectionModel = model->getCoinSelectionTableModel();

    proxyModelCoinSelectionZ = new QSortFilterProxyModel(this);
    proxyModelCoinSelectionZ->setSourceModel(coinSelectionModel);
    proxyModelCoinSelectionZ->setDynamicSortFilter(true);
    proxyModelCoinSelectionZ->setSortCaseSensitivity(Qt::CaseInsensitive);
    proxyModelCoinSelectionZ->setFilterCaseSensitivity(Qt::CaseInsensitive);

    proxyModelCoinSelectionT = new QSortFilterProxyModel(this);
    proxyModelCoinSelectionT->setSourceModel(coinSelectionModel);
    proxyModelCoinSelectionT->setDynamicSortFilter(true);
    proxyModelCoinSelectionT->setSortCaseSensitivity(Qt::CaseInsensitive);
    proxyModelCoinSelectionT->setFilterCaseSensitivity(Qt::CaseInsensitive);

    proxyModelCoinSelectionZ->setFilterRole(CoinSelectionTableModel::TypeRole);
    proxyModelCoinSelectionZ->setFilterFixedString(CoinSelectionTableModel::ZCoinSelection);
    ui->tableViewZ->setModel(proxyModelCoinSelectionZ);
    ui->tableViewZ->sortByColumn(0, Qt::AscendingOrder);

    proxyModelCoinSelectionT->setFilterRole(CoinSelectionTableModel::TypeRole);
    proxyModelCoinSelectionT->setFilterFixedString(CoinSelectionTableModel::TCoinSelection);
    ui->tableViewT->setModel(proxyModelCoinSelectionT);
    ui->tableViewT->sortByColumn(0, Qt::AscendingOrder);

    // Set column widths
#if QT_VERSION < 0x050000
    ui->tableViewZ->horizontalHeader()->setResizeMode(CoinSelectionTableModel::Address, QHeaderView::Stretch);
    ui->tableViewZ->horizontalHeader()->setResizeMode(CoinSelectionTableModel::Amount, QHeaderView::ResizeToContents);
    ui->tableViewT->horizontalHeader()->setResizeMode(CoinSelectionTableModel::Address, QHeaderView::Stretch);
    ui->tableViewT->horizontalHeader()->setResizeMode(CoinSelectionTableModel::Amount, QHeaderView::ResizeToContents);
#else
    ui->tableViewZ->horizontalHeader()->setSectionResizeMode(CoinSelectionTableModel::Address, QHeaderView::Stretch);
    ui->tableViewZ->horizontalHeader()->setSectionResizeMode(CoinSelectionTableModel::Amount, QHeaderView::ResizeToContents);
    ui->tableViewT->horizontalHeader()->setSectionResizeMode(CoinSelectionTableModel::Address, QHeaderView::Stretch);
    ui->tableViewT->horizontalHeader()->setSectionResizeMode(CoinSelectionTableModel::Amount, QHeaderView::ResizeToContents);
#endif

    connect(ui->tableViewZ->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
        this, SLOT(selectionCoinSelectionZChanged()));

    connect(ui->tableViewT->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
        this, SLOT(selectionCoinSelectionTChanged()));

    connect(model, SIGNAL(balanceChanged(CAmount,CAmount,CAmount,CAmount,CAmount,CAmount,CAmount,CAmount,CAmount)), this, SLOT(refreshTableModel()));

    refreshTableModel();
    selectionCoinSelectionZChanged();
    selectionCoinSelectionTChanged();
}

void CoinSelectionDialog::selectionCoinSelectionZChanged()
{
    // Set button states based on selected tab and selection
    QTableView *table = ui->tableViewZ;
    if(!table->selectionModel())
        return;

    if(table->selectionModel()->hasSelection())
    {
        ui->tableViewT->selectionModel()->clear();
    }
}

void CoinSelectionDialog::selectionCoinSelectionTChanged()
{
    // Set button states based on selected tab and selection
    QTableView *table = ui->tableViewT;
    if(!table->selectionModel())
        return;

    if(table->selectionModel()->hasSelection())
    {
        ui->tableViewZ->selectionModel()->clear();
    }
}

void CoinSelectionDialog::done(int retval)
{
    QTableView *tableT = ui->tableViewT;
    QTableView *tableZ = ui->tableViewZ;

    if((!tableT->selectionModel() || !tableT->model()) && (!tableZ->selectionModel() || !tableZ->model()))
        return;

    if(tableT->selectionModel()->hasSelection())
    {
        QModelIndex index = tableT->currentIndex();
        int rownumber = index.row();
        QModelIndex index1 = index.sibling(rownumber, 0);
        QModelIndex index2 = index.sibling(rownumber, 1);

        QVariant address = index1.data();
        returnAddress = address.toString();
        QVariant amount = index2.data();
        returnAmount = amount.toString();
    }

    if(tableZ->selectionModel()->hasSelection())
    {
        QModelIndex index = tableZ->currentIndex();
        int rownumber = index.row();
        QModelIndex index1 = index.sibling(rownumber, 0);
        QModelIndex index2 = index.sibling(rownumber, 1);

        QVariant address = index1.data();
        returnAddress = address.toString();
        QVariant amount = index2.data();
        returnAmount = amount.toString();
    }

    if(returnAddress.isEmpty())
    {
        // If no address entry selected, return rejected
        retval = Rejected;
    }

    QDialog::done(retval);
}

void CoinSelectionDialog::refreshTableModel()
{
    coinSelectionModel->refresh();
}


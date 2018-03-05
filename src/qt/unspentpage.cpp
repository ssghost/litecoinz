// Copyright (c) 2017-2018 The LitecoinZ developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#if defined(HAVE_CONFIG_H)
#include "config/bitcoin-config.h"
#endif

#include "unspentpage.h"
#include "ui_unspentpage.h"

#include "unspenttablemodel.h"
#include "bitcoinunits.h"
#include "clientmodel.h"
#include "optionsmodel.h"
#include "guiutil.h"
#include "platformstyle.h"
#include "walletmodel.h"

#include <QIcon>
#include <QMenu>
#include <QMessageBox>
#include <QSortFilterProxyModel>

UnspentPage::UnspentPage(const PlatformStyle *platformStyle, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UnspentPage),
    model(0),
    platformStyle(platformStyle)
{
    ui->setupUi(this);

    ui->tableViewZ->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableViewT->setEditTriggers(QAbstractItemView::NoEditTriggers);

    setWindowTitle(tr("Unspent transactions list"));

    // Context menu actions
    QAction *copyZAddressAction = new QAction(tr("&Copy Address"), this);
    QAction *copyZTxidAction = new QAction(tr("Copy &Transaction ID"), this);
    QAction *copyZAmountAction = new QAction(tr("Copy &Amount"), this);

    QAction *copyTAddressAction = new QAction(tr("&Copy Address"), this);
    QAction *copyTTxidAction = new QAction(tr("Copy &Transaction ID"), this);
    QAction *copyTAmountAction = new QAction(tr("Copy &Amount"), this);

    // Build context menu
    contextZMenu = new QMenu(this);
    contextZMenu->addAction(copyZAddressAction);
    contextZMenu->addAction(copyZTxidAction);
    contextZMenu->addAction(copyZAmountAction);
    contextZMenu->addSeparator();

    contextTMenu = new QMenu(this);
    contextTMenu->addAction(copyTAddressAction);
    contextTMenu->addAction(copyTTxidAction);
    contextTMenu->addAction(copyTAmountAction);
    contextTMenu->addSeparator();

    // Connect signals for context menu actions
    connect(copyZAddressAction, SIGNAL(triggered()), this, SLOT(onCopyZAddressAction()));
    connect(copyZTxidAction, SIGNAL(triggered()), this, SLOT(onCopyZTxidAction()));
    connect(copyZAmountAction, SIGNAL(triggered()), this, SLOT(onCopyZAmountAction()));

    connect(copyTAddressAction, SIGNAL(triggered()), this, SLOT(onCopyTAddressAction()));
    connect(copyTTxidAction, SIGNAL(triggered()), this, SLOT(onCopyTTxidAction()));
    connect(copyTAmountAction, SIGNAL(triggered()), this, SLOT(onCopyTAmountAction()));

    connect(ui->tableViewZ, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextualZMenu(QPoint)));
    connect(ui->tableViewT, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextualTMenu(QPoint)));
}

UnspentPage::~UnspentPage()
{
    delete ui;
}

void UnspentPage::onCopyZAddressAction()
{
    GUIUtil::copyEntryData(ui->tableViewZ, UnspentTableModel::Address);
}

void UnspentPage::onCopyZTxidAction()
{
    GUIUtil::copyEntryData(ui->tableViewZ, UnspentTableModel::Txid);
}

void UnspentPage::onCopyZAmountAction()
{
    GUIUtil::copyEntryData(ui->tableViewZ, UnspentTableModel::Amount);
}

void UnspentPage::onCopyTAddressAction()
{
    GUIUtil::copyEntryData(ui->tableViewT, UnspentTableModel::Address);
}

void UnspentPage::onCopyTTxidAction()
{
    GUIUtil::copyEntryData(ui->tableViewT, UnspentTableModel::Txid);
}

void UnspentPage::onCopyTAmountAction()
{
    GUIUtil::copyEntryData(ui->tableViewT, UnspentTableModel::Amount);
}

void UnspentPage::contextualZMenu(const QPoint &point)
{
    QModelIndex index = ui->tableViewZ->indexAt(point);
    if(index.isValid())
    {
        contextZMenu->exec(QCursor::pos());
    }
}

void UnspentPage::contextualTMenu(const QPoint &point)
{
    QModelIndex index = ui->tableViewT->indexAt(point);
    if(index.isValid())
    {
        contextTMenu->exec(QCursor::pos());
    }
}

void UnspentPage::setModel(WalletModel *model)
{
    this->model = model;
    if(!model)
        return;

    unspentModel = model->getUnspentTableModel();

    proxyModelUnspentZ = new QSortFilterProxyModel(this);
    proxyModelUnspentZ->setSourceModel(unspentModel);
    proxyModelUnspentZ->setDynamicSortFilter(true);
    proxyModelUnspentZ->setSortCaseSensitivity(Qt::CaseInsensitive);
    proxyModelUnspentZ->setFilterCaseSensitivity(Qt::CaseInsensitive);

    proxyModelUnspentT = new QSortFilterProxyModel(this);
    proxyModelUnspentT->setSourceModel(unspentModel);
    proxyModelUnspentT->setDynamicSortFilter(true);
    proxyModelUnspentT->setSortCaseSensitivity(Qt::CaseInsensitive);
    proxyModelUnspentT->setFilterCaseSensitivity(Qt::CaseInsensitive);

    proxyModelUnspentZ->setFilterRole(UnspentTableModel::TypeRole);
    proxyModelUnspentZ->setFilterFixedString(UnspentTableModel::ZUnspent);
    ui->tableViewZ->setModel(proxyModelUnspentZ);
    ui->tableViewZ->sortByColumn(0, Qt::AscendingOrder);

    proxyModelUnspentT->setFilterRole(UnspentTableModel::TypeRole);
    proxyModelUnspentT->setFilterFixedString(UnspentTableModel::TUnspent);
    ui->tableViewT->setModel(proxyModelUnspentT);
    ui->tableViewT->sortByColumn(0, Qt::AscendingOrder);

    // Set column widths
#if QT_VERSION < 0x050000
    ui->tableViewZ->horizontalHeader()->setResizeMode(UnspentTableModel::Address, QHeaderView::Stretch);
    ui->tableViewZ->horizontalHeader()->setResizeMode(UnspentTableModel::Txid, QHeaderView::ResizeToContents);
    ui->tableViewZ->horizontalHeader()->setResizeMode(UnspentTableModel::Amount, QHeaderView::ResizeToContents);
    ui->tableViewT->horizontalHeader()->setResizeMode(UnspentTableModel::Address, QHeaderView::Stretch);
    ui->tableViewT->horizontalHeader()->setResizeMode(UnspentTableModel::Txid, QHeaderView::ResizeToContents);
    ui->tableViewT->horizontalHeader()->setResizeMode(UnspentTableModel::Amount, QHeaderView::ResizeToContents);
#else
    ui->tableViewZ->horizontalHeader()->setSectionResizeMode(UnspentTableModel::Address, QHeaderView::Stretch);
    ui->tableViewZ->horizontalHeader()->setSectionResizeMode(UnspentTableModel::Txid, QHeaderView::ResizeToContents);
    ui->tableViewZ->horizontalHeader()->setSectionResizeMode(UnspentTableModel::Amount, QHeaderView::ResizeToContents);
    ui->tableViewT->horizontalHeader()->setSectionResizeMode(UnspentTableModel::Address, QHeaderView::Stretch);
    ui->tableViewT->horizontalHeader()->setSectionResizeMode(UnspentTableModel::Txid, QHeaderView::ResizeToContents);
    ui->tableViewT->horizontalHeader()->setSectionResizeMode(UnspentTableModel::Amount, QHeaderView::ResizeToContents);
#endif

    connect(ui->tableViewZ->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
        this, SLOT(selectionUnspentZChanged()));

    connect(ui->tableViewT->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
        this, SLOT(selectionUnspentTChanged()));

    connect(model, SIGNAL(balanceChanged(CAmount,CAmount,CAmount,CAmount,CAmount,CAmount,CAmount,CAmount,CAmount)), this, SLOT(refreshTableModel()));

    refreshTableModel();
    selectionUnspentZChanged();
    selectionUnspentTChanged();
}

void UnspentPage::selectionUnspentZChanged()
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

void UnspentPage::selectionUnspentTChanged()
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

void UnspentPage::refreshTableModel()
{
    unspentModel->refresh();
}

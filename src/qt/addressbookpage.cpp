// Copyright (c) 2011-2015 The Bitcoin Core developers
// Copyright (c) 2017-2018 The LitecoinZ developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#if defined(HAVE_CONFIG_H)
#include "config/bitcoin-config.h"
#endif

#include "addressbookpage.h"
#include "ui_addressbookpage.h"

#include "addresstablemodel.h"
#include "bitcoingui.h"
#include "csvmodelwriter.h"
#include "guiutil.h"
#include "editaddressdialog.h"
#include "platformstyle.h"

#include <QIcon>
#include <QMenu>
#include <QMessageBox>
#include <QSortFilterProxyModel>

AddressBookPage::AddressBookPage(const PlatformStyle *platformStyle, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddressBookPage),
    model(0),
    platformStyle(platformStyle)
{
    ui->setupUi(this);

    if (!platformStyle->getImagesOnButtons()) {
        ui->newReceivingZAddress->setIcon(QIcon());
        ui->newReceivingTAddress->setIcon(QIcon());
        ui->newSendingTAddress->setIcon(QIcon());

        ui->copyReceivingZAddress->setIcon(QIcon());
        ui->copyReceivingTAddress->setIcon(QIcon());
        ui->copySendingTAddress->setIcon(QIcon());

        ui->exportReceivingZAddress->setIcon(QIcon());
        ui->exportReceivingTAddress->setIcon(QIcon());
        ui->exportSendingTAddress->setIcon(QIcon());

        ui->deleteSendingTAddress->setIcon(QIcon());
    } else {
        ui->newReceivingZAddress->setIcon(platformStyle->MultiColorIcon(":/images/add1"));
        ui->newReceivingTAddress->setIcon(platformStyle->MultiColorIcon(":/images/add2"));
        ui->newSendingTAddress->setIcon(platformStyle->MultiColorIcon(":/images/add1"));

        ui->copyReceivingZAddress->setIcon(platformStyle->MultiColorIcon(":/images/copy"));
        ui->copyReceivingTAddress->setIcon(platformStyle->MultiColorIcon(":/images/copy"));
        ui->copySendingTAddress->setIcon(platformStyle->MultiColorIcon(":/images/copy"));

        ui->exportReceivingZAddress->setIcon(platformStyle->MultiColorIcon(":/images/export"));
        ui->exportReceivingTAddress->setIcon(platformStyle->MultiColorIcon(":/images/export"));
        ui->exportSendingTAddress->setIcon(platformStyle->MultiColorIcon(":/images/export"));

        ui->deleteSendingTAddress->setIcon(platformStyle->MultiColorIcon(":/images/remove1"));
    }

    // Set "Receiving Addresses" as current tab 
    ui->tabWidget->setCurrentIndex(0);

    // Set "Receiving Addresses View" not editable
    ui->tableViewReceivingZ->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableViewReceivingZ->setFocus();

    // Context menu actions
    QAction *copyReceivingZAddressAction = new QAction(tr("&Copy Address"), this);
    QAction *copyReceivingZLabelAction = new QAction(tr("Copy &Label"), this);

    QAction *copyReceivingTAddressAction = new QAction(tr("&Copy Address"), this);
    QAction *copyReceivingTLabelAction = new QAction(tr("Copy &Label"), this);

    QAction *copySendingTAddressAction = new QAction(tr("&Copy Address"), this);
    QAction *copySendingTLabelAction = new QAction(tr("Copy &Label"), this);

    QAction *editReceivingTAction = new QAction(tr("&Edit"), this);
    QAction *editSendingTAction = new QAction(tr("&Edit"), this);

    deleteSendingTAction = new QAction(ui->deleteSendingTAddress->text(), this);

    // Build context menu
    contextReceivingZMenu = new QMenu(this);
    contextReceivingZMenu->addAction(copyReceivingZAddressAction);
    contextReceivingZMenu->addAction(copyReceivingZLabelAction);
    contextReceivingZMenu->addSeparator();

    contextReceivingTMenu = new QMenu(this);
    contextReceivingTMenu->addAction(copyReceivingTAddressAction);
    contextReceivingTMenu->addAction(copyReceivingTLabelAction);
    contextReceivingTMenu->addAction(editReceivingTAction);
    contextReceivingTMenu->addSeparator();

    contextSendingTMenu = new QMenu(this);
    contextSendingTMenu->addAction(copySendingTAddressAction);
    contextSendingTMenu->addAction(copySendingTLabelAction);
    contextSendingTMenu->addAction(editSendingTAction);
    contextSendingTMenu->addAction(deleteSendingTAction);
    contextSendingTMenu->addSeparator();

    // Connect signals for context menu actions
    connect(copyReceivingZAddressAction, SIGNAL(triggered()), this, SLOT(on_copyReceivingZAddress_clicked()));
    connect(copyReceivingTAddressAction, SIGNAL(triggered()), this, SLOT(on_copyReceivingTAddress_clicked()));
    connect(copySendingTAddressAction, SIGNAL(triggered()), this, SLOT(on_copySendingTAddress_clicked()));

    connect(copyReceivingZLabelAction, SIGNAL(triggered()), this, SLOT(onCopyLabelReceivingZAction()));
    connect(copyReceivingTLabelAction, SIGNAL(triggered()), this, SLOT(onCopyLabelReceivingTAction()));
    connect(copySendingTLabelAction, SIGNAL(triggered()), this, SLOT(onCopyLabelSendingTAction()));

    connect(editReceivingTAction, SIGNAL(triggered()), this, SLOT(onEditReceivingTAction()));
    connect(editSendingTAction, SIGNAL(triggered()), this, SLOT(onEditSendingTAction()));

    connect(deleteSendingTAction, SIGNAL(triggered()), this, SLOT(on_deleteSendingTAddress_clicked()));

    connect(ui->tableViewReceivingZ, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextualReceivingZMenu(QPoint)));
    connect(ui->tableViewReceivingT, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextualReceivingTMenu(QPoint)));
    connect(ui->tableViewSendingT, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextualSendingTMenu(QPoint)));
}

void AddressBookPage::onCopyLabelReceivingZAction()
{
    GUIUtil::copyEntryData(ui->tableViewReceivingZ, AddressTableModel::Label);
}

void AddressBookPage::onCopyLabelReceivingTAction()
{
    GUIUtil::copyEntryData(ui->tableViewReceivingT, AddressTableModel::Label);
}

void AddressBookPage::onCopyLabelSendingTAction()
{
    GUIUtil::copyEntryData(ui->tableViewSendingT, AddressTableModel::Label);
}

void AddressBookPage::onEditReceivingTAction()
{
    if(!model)
        return;

    if(!ui->tableViewReceivingT->selectionModel())
        return;
    QModelIndexList indexes = ui->tableViewReceivingT->selectionModel()->selectedRows();
    if(indexes.isEmpty())
        return;

    EditAddressDialog dlg(EditAddressDialog::EditReceivingAddress, this);
    dlg.setModel(model);
    QModelIndex origIndex = proxyModelReceivingT->mapToSource(indexes.at(0));
    dlg.loadRow(origIndex.row());
    dlg.exec();
}

void AddressBookPage::onEditSendingTAction()
{
    if(!model)
        return;

    if(!ui->tableViewSendingT->selectionModel())
        return;
    QModelIndexList indexes = ui->tableViewSendingT->selectionModel()->selectedRows();
    if(indexes.isEmpty())
        return;

    EditAddressDialog dlg(EditAddressDialog::EditSendingAddress, this);
    dlg.setModel(model);
    QModelIndex origIndex = proxyModelSendingT->mapToSource(indexes.at(0));
    dlg.loadRow(origIndex.row());
    dlg.exec();
}

void AddressBookPage::contextualReceivingZMenu(const QPoint &point)
{
    QModelIndex index = ui->tableViewReceivingZ->indexAt(point);
    if(index.isValid())
    {
        contextReceivingZMenu->exec(QCursor::pos());
    }
}

void AddressBookPage::contextualReceivingTMenu(const QPoint &point)
{
    QModelIndex index = ui->tableViewReceivingT->indexAt(point);
    if(index.isValid())
    {
        contextReceivingTMenu->exec(QCursor::pos());
    }
}

void AddressBookPage::contextualSendingTMenu(const QPoint &point)
{
    QModelIndex index = ui->tableViewSendingT->indexAt(point);
    if(index.isValid())
    {
        contextSendingTMenu->exec(QCursor::pos());
    }
}

void AddressBookPage::setModel(AddressTableModel *model)
{
    this->model = model;

    if(!model)
        return;

    proxyModelReceivingZ = new QSortFilterProxyModel(this);
    proxyModelReceivingZ->setSourceModel(model);
    proxyModelReceivingZ->setDynamicSortFilter(true);
    proxyModelReceivingZ->setSortCaseSensitivity(Qt::CaseInsensitive);
    proxyModelReceivingZ->setFilterCaseSensitivity(Qt::CaseInsensitive);

    proxyModelReceivingT = new QSortFilterProxyModel(this);
    proxyModelReceivingT->setSourceModel(model);
    proxyModelReceivingT->setDynamicSortFilter(true);
    proxyModelReceivingT->setSortCaseSensitivity(Qt::CaseInsensitive);
    proxyModelReceivingT->setFilterCaseSensitivity(Qt::CaseInsensitive);

    proxyModelSendingT = new QSortFilterProxyModel(this);
    proxyModelSendingT->setSourceModel(model);
    proxyModelSendingT->setDynamicSortFilter(true);
    proxyModelSendingT->setSortCaseSensitivity(Qt::CaseInsensitive);
    proxyModelSendingT->setFilterCaseSensitivity(Qt::CaseInsensitive);

    // Receive filter (z-addresses)
    proxyModelReceivingZ->setFilterRole(AddressTableModel::TypeRole);
    proxyModelReceivingZ->setFilterFixedString(AddressTableModel::ZReceive);
    ui->tableViewReceivingZ->setModel(proxyModelReceivingZ);
    ui->tableViewReceivingZ->sortByColumn(0, Qt::AscendingOrder);

    // Receive filter (t-addresses)
    proxyModelReceivingT->setFilterRole(AddressTableModel::TypeRole);
    proxyModelReceivingT->setFilterFixedString(AddressTableModel::Receive);
    ui->tableViewReceivingT->setModel(proxyModelReceivingT);
    ui->tableViewReceivingT->sortByColumn(0, Qt::AscendingOrder);

    // Send filter
    proxyModelSendingT->setFilterRole(AddressTableModel::TypeRole);
    proxyModelSendingT->setFilterFixedString(AddressTableModel::Send);
    ui->tableViewSendingT->setModel(proxyModelSendingT);
    ui->tableViewSendingT->sortByColumn(0, Qt::AscendingOrder);

    // Set column widths
#if QT_VERSION < 0x050000
    ui->tableViewReceivingZ->horizontalHeader()->setResizeMode(AddressTableModel::Label, QHeaderView::Stretch);
    ui->tableViewReceivingZ->horizontalHeader()->setResizeMode(AddressTableModel::Address, QHeaderView::ResizeToContents);
    ui->tableViewReceivingT->horizontalHeader()->setResizeMode(AddressTableModel::Label, QHeaderView::Stretch);
    ui->tableViewReceivingT->horizontalHeader()->setResizeMode(AddressTableModel::Address, QHeaderView::ResizeToContents);
    ui->tableViewSendingT->horizontalHeader()->setResizeMode(AddressTableModel::Label, QHeaderView::Stretch);
    ui->tableViewSendingT->horizontalHeader()->setResizeMode(AddressTableModel::Address, QHeaderView::ResizeToContents);
#else
    ui->tableViewReceivingZ->horizontalHeader()->setSectionResizeMode(AddressTableModel::Label, QHeaderView::Stretch);
    ui->tableViewReceivingZ->horizontalHeader()->setSectionResizeMode(AddressTableModel::Address, QHeaderView::ResizeToContents);
    ui->tableViewReceivingT->horizontalHeader()->setSectionResizeMode(AddressTableModel::Label, QHeaderView::Stretch);
    ui->tableViewReceivingT->horizontalHeader()->setSectionResizeMode(AddressTableModel::Address, QHeaderView::ResizeToContents);
    ui->tableViewSendingT->horizontalHeader()->setSectionResizeMode(AddressTableModel::Label, QHeaderView::Stretch);
    ui->tableViewSendingT->horizontalHeader()->setSectionResizeMode(AddressTableModel::Address, QHeaderView::ResizeToContents);
#endif

    connect(ui->tableViewReceivingZ->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
        this, SLOT(selectionReceivingZChanged()));
    connect(ui->tableViewReceivingT->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
        this, SLOT(selectionReceivingTChanged()));
    connect(ui->tableViewSendingT->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
        this, SLOT(selectionSendingTChanged()));

    // Select row for newly created address
    connect(model, SIGNAL(rowsInserted(QModelIndex,int,int)), this, SLOT(selectNewAddress(QModelIndex,int,int)));

    selectionReceivingZChanged();
    selectionReceivingTChanged();
    selectionSendingTChanged();
}

AddressBookPage::~AddressBookPage()
{
    delete ui;
}


// Receiving Addresses Tab
void AddressBookPage::on_newReceivingZAddress_clicked()
{
    newReceivingZAddressToSelect = model->addRow(AddressTableModel::ZReceive, "", "");
}

void AddressBookPage::on_copyReceivingZAddress_clicked()
{
    GUIUtil::copyEntryData(ui->tableViewReceivingZ, AddressTableModel::Address);
}

void AddressBookPage::on_exportReceivingZAddress_clicked()
{
    // CSV is currently the only supported format
    QString filename = GUIUtil::getSaveFileName(this,
        tr("Export Address List"), QString(),
        tr("Comma separated file (*.csv)"), NULL);

    if (filename.isNull())
        return;

    CSVModelWriter writer(filename);

    // name, column, role
    writer.setModel(proxyModelReceivingZ);
    writer.addColumn("Label", AddressTableModel::Label, Qt::EditRole);
    writer.addColumn("Address", AddressTableModel::Address, Qt::EditRole);

    if(!writer.write()) {
        QMessageBox::critical(this, tr("Exporting Failed"),
            tr("There was an error trying to save the address list to %1. Please try again.").arg(filename));
    }
}

void AddressBookPage::on_newReceivingTAddress_clicked()
{
    if(!model)
        return;

    EditAddressDialog dlg(EditAddressDialog::NewReceivingAddress, this);
    dlg.setModel(model);
    if(dlg.exec())
    {
        newReceivingTAddressToSelect = dlg.getAddress();
    }
}

void AddressBookPage::on_copyReceivingTAddress_clicked()
{
    GUIUtil::copyEntryData(ui->tableViewReceivingT, AddressTableModel::Address);
}

void AddressBookPage::on_exportReceivingTAddress_clicked()
{
    // CSV is currently the only supported format
    QString filename = GUIUtil::getSaveFileName(this,
        tr("Export Address List"), QString(),
        tr("Comma separated file (*.csv)"), NULL);

    if (filename.isNull())
        return;

    CSVModelWriter writer(filename);

    // name, column, role
    writer.setModel(proxyModelReceivingT);
    writer.addColumn("Label", AddressTableModel::Label, Qt::EditRole);
    writer.addColumn("Address", AddressTableModel::Address, Qt::EditRole);

    if(!writer.write()) {
        QMessageBox::critical(this, tr("Exporting Failed"),
            tr("There was an error trying to save the address list to %1. Please try again.").arg(filename));
    }
}


// Sending Addresses Tab
void AddressBookPage::on_newSendingTAddress_clicked()
{
    if(!model)
        return;

    EditAddressDialog dlg(EditAddressDialog::NewSendingAddress, this);
    dlg.setModel(model);
    if(dlg.exec())
    {
        newSendingTAddressToSelect = dlg.getAddress();
    }
}

void AddressBookPage::on_copySendingTAddress_clicked()
{
    GUIUtil::copyEntryData(ui->tableViewSendingT, AddressTableModel::Address);
}

void AddressBookPage::on_deleteSendingTAddress_clicked()
{
    QTableView *table = ui->tableViewSendingT;
    if(!table->selectionModel())
        return;

    QModelIndexList indexes = table->selectionModel()->selectedRows();
    if(!indexes.isEmpty())
    {
        table->model()->removeRow(indexes.at(0).row());
    }
}

void AddressBookPage::on_exportSendingTAddress_clicked()
{
    // CSV is currently the only supported format
    QString filename = GUIUtil::getSaveFileName(this,
        tr("Export Address List"), QString(),
        tr("Comma separated file (*.csv)"), NULL);

    if (filename.isNull())
        return;

    CSVModelWriter writer(filename);

    // name, column, role
    writer.setModel(proxyModelSendingT);
    writer.addColumn("Label", AddressTableModel::Label, Qt::EditRole);
    writer.addColumn("Address", AddressTableModel::Address, Qt::EditRole);

    if(!writer.write()) {
        QMessageBox::critical(this, tr("Exporting Failed"),
            tr("There was an error trying to save the address list to %1. Please try again.").arg(filename));
    }
}

void AddressBookPage::selectionReceivingZChanged()
{
    // Set button states based on selection
    QTableView *table = ui->tableViewReceivingZ;
    if(!table->selectionModel())
        return;

    if(table->selectionModel()->hasSelection())
    {
        ui->tableViewReceivingT->selectionModel()->clear();
        ui->tableViewSendingT->selectionModel()->clear();
        ui->copyReceivingZAddress->setEnabled(true);
    }
    else
    {
        ui->copyReceivingZAddress->setEnabled(false);
    }
}

void AddressBookPage::selectionReceivingTChanged()
{
    // Set button states based on selection
    QTableView *table = ui->tableViewReceivingT;
    if(!table->selectionModel())
        return;

    if(table->selectionModel()->hasSelection())
    {
        ui->tableViewReceivingZ->selectionModel()->clear();
        ui->tableViewSendingT->selectionModel()->clear();
        ui->copyReceivingTAddress->setEnabled(true);
    }
    else
    {
        ui->copyReceivingTAddress->setEnabled(false);
    }
}

void AddressBookPage::selectionSendingTChanged()
{
    // Set button states based on selection
    QTableView *table = ui->tableViewSendingT;
    if(!table->selectionModel())
        return;

    if(table->selectionModel()->hasSelection())
    {
        ui->tableViewReceivingZ->selectionModel()->clear();
        ui->tableViewReceivingT->selectionModel()->clear();
        ui->copySendingTAddress->setEnabled(true);
        ui->deleteSendingTAddress->setEnabled(true);
    }
    else
    {
        ui->copySendingTAddress->setEnabled(false);
        ui->deleteSendingTAddress->setEnabled(false);
    }
}

void AddressBookPage::selectNewAddress(const QModelIndex &parent, int begin, int /*end*/)
{
    QModelIndex idxReceivingZ = proxyModelReceivingZ->mapFromSource(model->index(begin, AddressTableModel::Address, parent));
    QModelIndex idxReceivingT = proxyModelReceivingT->mapFromSource(model->index(begin, AddressTableModel::Address, parent));
    QModelIndex idxSendingT = proxyModelSendingT->mapFromSource(model->index(begin, AddressTableModel::Address, parent));

    if(idxReceivingZ.isValid() && (idxReceivingZ.data(Qt::EditRole).toString() == newReceivingZAddressToSelect))
    {
        // Select row of newly created address, once
        ui->tableViewReceivingZ->setFocus();
        ui->tableViewReceivingZ->selectRow(idxReceivingZ.row());
        newReceivingZAddressToSelect.clear();
    }

    if(idxReceivingT.isValid() && (idxReceivingT.data(Qt::EditRole).toString() == newReceivingTAddressToSelect))
    {
        // Select row of newly created address, once
        ui->tableViewReceivingT->setFocus();
        ui->tableViewReceivingT->selectRow(idxReceivingT.row());
        newReceivingTAddressToSelect.clear();
    }

    if(idxSendingT.isValid() && (idxSendingT.data(Qt::EditRole).toString() == newSendingTAddressToSelect))
    {
        // Select row of newly created address, once
        ui->tableViewSendingT->setFocus();
        ui->tableViewSendingT->selectRow(idxSendingT.row());
        newSendingTAddressToSelect.clear();
    }
}

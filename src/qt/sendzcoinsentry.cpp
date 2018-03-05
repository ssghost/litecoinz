// Copyright (c) 2011-2015 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "sendzcoinsentry.h"
#include "ui_sendzcoinsentry.h"

#include "addressbookdialog.h"
#include "addresstablemodel.h"
#include "guiutil.h"
#include "optionsmodel.h"
#include "platformstyle.h"
#include "walletmodel.h"

#include <QApplication>
#include <QClipboard>

SendZCoinsEntry::SendZCoinsEntry(const PlatformStyle *platformStyle, QWidget *parent) :
    QStackedWidget(parent),
    ui(new Ui::SendZCoinsEntry),
    model(0),
    platformStyle(platformStyle)
{
    ui->setupUi(this);

    ui->addAsMemo->setVisible(false);
    ui->memoLabel->setVisible(false);

    if (!platformStyle->getImagesOnButtons()) {
        ui->addressBookButton->setIcon(QIcon());
        ui->pasteButton->setIcon(QIcon());
        ui->deleteButton->setIcon(QIcon());
        ui->deleteButton_is->setIcon(QIcon());
        ui->deleteButton_s->setIcon(QIcon());
    } else {
        ui->addressBookButton->setIcon(platformStyle->MultiColorIcon(":/images/addressbook1"));
        ui->pasteButton->setIcon(platformStyle->MultiColorIcon(":/images/paste"));
        ui->deleteButton->setIcon(platformStyle->MultiColorIcon(":/images/clear"));
        ui->deleteButton_is->setIcon(platformStyle->MultiColorIcon(":/images/clear"));
        ui->deleteButton_s->setIcon(platformStyle->MultiColorIcon(":/images/clear"));
    }

    setCurrentWidget(ui->SendZCoins);

    if (platformStyle->getUseExtraSpacing())
        ui->payToLayout->setSpacing(4);
#if QT_VERSION >= 0x040700
    ui->addAsLabel->setPlaceholderText(tr("Enter a label for this address to add it to your address book"));
#endif

    // normal litecoinz address field
    GUIUtil::setupAddressWidget(ui->payTo, this);
    // just a label for displaying litecoinz address(es)
    ui->payTo_is->setFont(GUIUtil::fixedPitchFont());

    // Connect signals
    connect(ui->payAmount, SIGNAL(valueChanged()), this, SIGNAL(payAmountChanged()));
    connect(ui->deleteButton, SIGNAL(clicked()), this, SLOT(deleteClicked()));
    connect(ui->deleteButton_is, SIGNAL(clicked()), this, SLOT(deleteClicked()));
    connect(ui->deleteButton_s, SIGNAL(clicked()), this, SLOT(deleteClicked()));

    connect(ui->checkboxSendToZAddress, SIGNAL(stateChanged(int)), this, SLOT(sendToZAddressChangeChecked(int)));
}

SendZCoinsEntry::~SendZCoinsEntry()
{
    delete ui;
}

void SendZCoinsEntry::on_pasteButton_clicked()
{
    // Paste text from clipboard into recipient field
    ui->payTo->setText(QApplication::clipboard()->text());
}

void SendZCoinsEntry::on_addressBookButton_clicked()
{
    if(!model)
        return;
    AddressBookDialog dlg(platformStyle, AddressBookDialog::ForTSelection, AddressBookDialog::SendingTab, this);
    dlg.setModel(model->getAddressTableModel());
    if(dlg.exec())
    {
        ui->payTo->setText(dlg.getReturnValue());
        ui->payAmount->setFocus();
    }
}

void SendZCoinsEntry::on_payTo_textChanged(const QString &address)
{
    updateLabel(address);
}

void SendZCoinsEntry::setModel(WalletModel *model)
{
    this->model = model;

    if (model && model->getOptionsModel())
        connect(model->getOptionsModel(), SIGNAL(displayUnitChanged(int)), this, SLOT(updateDisplayUnit()));

    clear();
}

void SendZCoinsEntry::clear()
{
    // clear UI elements for normal payment
    ui->payTo->clear();
    ui->addAsLabel->clear();
    ui->addAsMemo->clear();
    ui->payAmount->clear();
    ui->checkboxSendToZAddress->setCheckState(Qt::Unchecked);
    // clear UI elements for unauthenticated payment request
    ui->payTo_is->clear();
    ui->payAmount_is->clear();
    // clear UI elements for authenticated payment request
    ui->payTo_s->clear();
    ui->payAmount_s->clear();

    // update the display unit, to not use the default ("LTZ")
    updateDisplayUnit();
}

void SendZCoinsEntry::deleteClicked()
{
    Q_EMIT removeEntry(this);
}

bool SendZCoinsEntry::validate()
{
    if (!model)
        return false;

    // Check input validity
    bool retval = true;

    // Skip checks for payment request
    if (recipient.paymentRequest.IsInitialized())
        return retval;

    if ((!model->validateAddress(ui->payTo->text())) && (!model->validateZAddress(ui->payTo->text())))
    {
        ui->payTo->setValid(false);
        retval = false;
    }

    if (!ui->payAmount->validate())
    {
        retval = false;
    }

    // Sending a zero amount is invalid
    if (ui->payAmount->value(0) <= 0)
    {
        ui->payAmount->setValid(false);
        retval = false;
    }

    // Reject dust outputs:
    if (retval && GUIUtil::isDust(ui->payTo->text(), ui->payAmount->value())) {
        ui->payAmount->setValid(false);
        retval = false;
    }

    return retval;
}

SendCoinsRecipient SendZCoinsEntry::getValue()
{
    // Payment request
    if (recipient.paymentRequest.IsInitialized())
        return recipient;

    // Normal payment
    recipient.address = ui->payTo->text();
    recipient.label = ui->addAsLabel->text();
    recipient.message = ui->addAsMemo->text();
    recipient.amount = ui->payAmount->value();

    return recipient;
}

QWidget *SendZCoinsEntry::setupTabChain(QWidget *prev)
{
    QWidget::setTabOrder(prev, ui->payTo);
    QWidget::setTabOrder(ui->payTo, ui->addAsLabel);
    QWidget::setTabOrder(ui->addAsLabel, ui->addAsMemo);
    QWidget *w = ui->payAmount->setupTabChain(ui->addAsMemo);
    QWidget::setTabOrder(w, ui->checkboxSendToZAddress);
    QWidget::setTabOrder(ui->checkboxSendToZAddress, ui->addressBookButton);
    QWidget::setTabOrder(ui->addressBookButton, ui->pasteButton);
    QWidget::setTabOrder(ui->pasteButton, ui->deleteButton);
    return ui->deleteButton;
}

void SendZCoinsEntry::setValue(const SendCoinsRecipient &value)
{
    recipient = value;

    if (recipient.paymentRequest.IsInitialized()) // payment request
    {
        if (recipient.authenticatedMerchant.isEmpty()) // unauthenticated
        {
            ui->payTo_is->setText(recipient.address);
            ui->payAmount_is->setValue(recipient.amount);
            ui->payAmount_is->setReadOnly(true);
            setCurrentWidget(ui->SendZCoins_UnauthenticatedPaymentRequest);
        }
        else // authenticated
        {
            ui->payTo_s->setText(recipient.authenticatedMerchant);
            ui->payAmount_s->setValue(recipient.amount);
            ui->payAmount_s->setReadOnly(true);
            setCurrentWidget(ui->SendZCoins_AuthenticatedPaymentRequest);
        }
    }
    else // normal payment
    {
        ui->addAsLabel->clear();
        ui->addAsMemo->clear();
        ui->payTo->setText(recipient.address); // this may set a label from addressbook
        if (!recipient.label.isEmpty()) // if a label had been set from the addressbook, don't overwrite with an empty label
            ui->addAsLabel->setText(recipient.label);
        ui->payAmount->setValue(recipient.amount);
    }
}

void SendZCoinsEntry::setAddress(const QString &address)
{
    ui->payTo->setText(address);
    ui->payAmount->setFocus();
}

bool SendZCoinsEntry::isClear()
{
    return ui->payTo->text().isEmpty() && ui->payTo_is->text().isEmpty() && ui->payTo_s->text().isEmpty();
}

void SendZCoinsEntry::setFocus()
{
    ui->payTo->setFocus();
}

void SendZCoinsEntry::updateDisplayUnit()
{
    if(model && model->getOptionsModel())
    {
        // Update payAmount with the current unit
        ui->payAmount->setDisplayUnit(model->getOptionsModel()->getDisplayUnit());
        ui->payAmount_is->setDisplayUnit(model->getOptionsModel()->getDisplayUnit());
        ui->payAmount_s->setDisplayUnit(model->getOptionsModel()->getDisplayUnit());
    }
}

bool SendZCoinsEntry::updateLabel(const QString &address)
{
    if(!model)
        return false;

    if(ui->checkboxSendToZAddress->checkState() == Qt::Checked)
        return false;

    // Fill in label from address book, if address has an associated label
    QString associatedLabel = model->getAddressTableModel()->labelForAddress(address);
    if(!associatedLabel.isEmpty())
    {
        ui->addAsLabel->setText(associatedLabel);
        return true;
    }

    return false;
}

void SendZCoinsEntry::sendToZAddressChangeChecked(int state)
{
    ui->addAsLabel->clear();
    ui->addAsMemo->clear();

    if (state == Qt::Checked)
    {
        ui->addAsLabel->setVisible(false);
        ui->labellLabel->setVisible(false);

        ui->addAsMemo->setVisible(true);
        ui->memoLabel->setVisible(true);

        ui->addressBookButton->setEnabled(false);
        ui->payTo->clear();
        GUIUtil::setupZAddressWidget(ui->payTo, this);
    }
    else
    {
        ui->addAsLabel->setVisible(true);
        ui->labellLabel->setVisible(true);

        ui->addAsMemo->setVisible(false);
        ui->memoLabel->setVisible(false);

        ui->addressBookButton->setEnabled(true);
        ui->payTo->clear();
        GUIUtil::setupAddressWidget(ui->payTo, this);
    }
}

// Copyright (c) 2017-2018 The LitecoinZ developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#if defined(HAVE_CONFIG_H)
#include "config/bitcoin-config.h"
#endif

#include "resultsdialog.h"
#include "ui_resultsdialog.h"
#include "platformstyle.h"

#include <univalue.h>
#include "rpc/server.h"

#include <QTimer>
#include <QMessageBox>

ResultsDialog::ResultsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResultsDialog)
{
    ui->setupUi(this);

    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint | Qt::WindowTitleHint);

    ui->buttonBox->hide();
    ui->progressBar->hide();

    ui->labelRecord->setVisible(false);
    ui->labelRecordText->setVisible(false);

    ui->labelResultText->setText("queued");
    ui->labelResultText->setStyleSheet("QLabel { color : blue; }");

    QSize qs = sizeHint();
    resize(qs);
}

int ResultsDialog::exec()
{
    counter = 0;
    skipEsc = true;

    ui->progressBar->setValue(counter);
    ui->progressBar->setMinimum(0);
    ui->progressBar->show();

    connect(&timer, SIGNAL(timeout()), this, SLOT(updateProgressBar()));
    timer.start(1000);
    return QDialog::exec();
}

void ResultsDialog::reject()
{
    if(!skipEsc)
        QDialog::reject();
}

void ResultsDialog::setOperationId(QString opid)
{
    strOperationId = opid;
    ui->labelOperationIdText->setText(strOperationId);
}

void ResultsDialog::setLabels(QString label1, QString label2)
{
    ui->labelRecord->setText(label1);
    ui->labelRecordText->setText(label2);

    ui->labelRecord->setVisible(true);
    ui->labelRecordText->setVisible(true);

    ui->labelRecord->setTextFormat(Qt::RichText);
    ui->labelRecordText->setTextFormat(Qt::RichText);

    QSize qs = sizeHint();
    resize(qs);
}

void ResultsDialog::updateProgressBar()
{
    try {
        /* Check and display the operation status */
        UniValue obj(UniValue::VARR);
        UniValue params(UniValue::VARR);
        obj.push_back(strOperationId.toStdString());
        params.push_back(obj);
        UniValue ret = z_getoperationstatus(params, false);

        UniValue status = find_value(ret[0], "status");
        strStatus = QString::fromStdString(status.get_str());

        if(strStatus == "failed")
        {
            UniValue err = find_value(ret[0], "error");
            UniValue errMessage = find_value(err, "message");
            ui->labelResultText->setText(QString::fromStdString(errMessage.get_str()));
            ui->labelResultText->setStyleSheet("QLabel { color : red; }");
        }
        else if (strStatus == "success")
        {
            ui->labelResultText->setStyleSheet("QLabel { color : green; }");
            ui->labelResultText->setText(strStatus);
        }
        else
        {
            ui->labelResultText->setStyleSheet("QLabel { color : blue; }");
            ui->labelResultText->setText(strStatus);
        }
    } catch (std::exception &e) {
        strStatus = "Ops... an internal error occurred!";
        qDebug("Error %s ", e.what());
        QMessageBox msgBox("", e.what(), QMessageBox::Critical, 0, 0, 0, this, Qt::WindowTitleHint | Qt::WindowSystemMenuHint);
        msgBox.exec();
    } catch (...) {
        strStatus = "Ops... an internal error occurred!";
        qDebug("Error <unknown>");
        QMessageBox msgBox("", "Error <unknown>", QMessageBox::Critical, 0, 0, 0, this, Qt::WindowTitleHint | Qt::WindowSystemMenuHint);
        msgBox.exec();
    }

    if((strStatus == "executing") || (strStatus == "queued"))
    {
        counter++;
        ui->progressBar->setValue(counter);
    }
    else
    {
        timer.stop();
        ui->progressBar->setValue(100);
        ui->progressBar->hide();
        ui->buttonBox->show();
        skipEsc = false;
    }
}

QString ResultsDialog::getResultStatus() const
{
    return strStatus;
}

ResultsDialog::~ResultsDialog()
{
    delete ui;
}

void ResultsDialog::on_buttonBox_clicked()
{
    QDialog::done(QDialog::Accepted);
}

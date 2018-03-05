// Copyright (c) 2017-2018 The LitecoinZ developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_QT_RESULTSDIALOG_H
#define BITCOIN_QT_RESULTSDIALOG_H

#include "platformstyle.h"
#include <QDialog>
#include <QTimer>
#include <QProgressDialog>

namespace Ui {
    class ResultsDialog;
}

/** Dialog for editing an address and associated information.
 */
class ResultsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ResultsDialog(QWidget *parent);
    ~ResultsDialog();

    void setOperationId(QString opid);
    void setLabels(QString label1, QString label2);

    QString getResultStatus() const;

public Q_SLOTS:
    void on_buttonBox_clicked();
    int exec() override;
    void reject() override;

private:
    Ui::ResultsDialog *ui;
    QString strOperationId;
    QString strStatus;

    int counter;
    QTimer timer;

    bool skipEsc;

private Q_SLOTS:
    void updateProgressBar();
};

#endif // BITCOIN_QT_RESULTSDIALOG_H

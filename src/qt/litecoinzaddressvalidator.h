// Copyright (c) 2017-2018 The LitecoinZ developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef BITCOIN_QT_LITECOINZADDRESSVALIDATOR_H
#define BITCOIN_QT_LITECOINZADDRESSVALIDATOR_H

#include <QValidator>

/** Base58 entry widget validator, checks for valid characters and
 * removes some whitespace.
 */
class LitecoinZAddressEntryValidator : public QValidator
{
    Q_OBJECT

public:
    explicit LitecoinZAddressEntryValidator(QObject *parent);

    State validate(QString &input, int &pos) const;
};

/** LitecoinZ address widget validator, checks for a valid litecoinz address.
 */
class LitecoinZAddressCheckValidator : public QValidator
{
    Q_OBJECT

public:
    explicit LitecoinZAddressCheckValidator(QObject *parent);

    State validate(QString &input, int &pos) const;
};

#endif // BITCOIN_QT_LITECOINZADDRESSVALIDATOR_H

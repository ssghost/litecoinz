// Copyright (c) 2009-2014 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include <qt/test/uritests.h>

#include <guiutil.h>
#include <walletmodel.h>

#include <QUrl>

void URITests::uriTests()
{
    SendCoinsRecipient rv;
    QUrl uri;
    uri.setUrl(QString("litecoinz:L1SdUmxUpkrnRG5hBsdtXzjonYtQ8sjcSYG?req-dontexist="));
    QVERIFY(!GUIUtil::parseBitcoinURI(uri, &rv));

    uri.setUrl(QString("litecoinz:L1SdUmxUpkrnRG5hBsdtXzjonYtQ8sjcSYG?dontexist="));
    QVERIFY(GUIUtil::parseBitcoinURI(uri, &rv));
    QVERIFY(rv.address == QString("L1SdUmxUpkrnRG5hBsdtXzjonYtQ8sjcSYG"));
    QVERIFY(rv.label == QString());
    QVERIFY(rv.amount == 0);

    uri.setUrl(QString("litecoinz:L1SdUmxUpkrnRG5hBsdtXzjonYtQ8sjcSYG?label=Wikipedia Example Address"));
    QVERIFY(GUIUtil::parseBitcoinURI(uri, &rv));
    QVERIFY(rv.address == QString("L1SdUmxUpkrnRG5hBsdtXzjonYtQ8sjcSYG"));
    QVERIFY(rv.label == QString("Wikipedia Example Address"));
    QVERIFY(rv.amount == 0);

    uri.setUrl(QString("litecoinz:L1SdUmxUpkrnRG5hBsdtXzjonYtQ8sjcSYG?amount=0.001"));
    QVERIFY(GUIUtil::parseBitcoinURI(uri, &rv));
    QVERIFY(rv.address == QString("L1SdUmxUpkrnRG5hBsdtXzjonYtQ8sjcSYG"));
    QVERIFY(rv.label == QString());
    QVERIFY(rv.amount == 100000);

    uri.setUrl(QString("litecoinz:L1SdUmxUpkrnRG5hBsdtXzjonYtQ8sjcSYG?amount=1.001"));
    QVERIFY(GUIUtil::parseBitcoinURI(uri, &rv));
    QVERIFY(rv.address == QString("L1SdUmxUpkrnRG5hBsdtXzjonYtQ8sjcSYG"));
    QVERIFY(rv.label == QString());
    QVERIFY(rv.amount == 100100000);

    uri.setUrl(QString("litecoinz:L1SdUmxUpkrnRG5hBsdtXzjonYtQ8sjcSYG?amount=100&label=Wikipedia Example"));
    QVERIFY(GUIUtil::parseBitcoinURI(uri, &rv));
    QVERIFY(rv.address == QString("L1SdUmxUpkrnRG5hBsdtXzjonYtQ8sjcSYG"));
    QVERIFY(rv.amount == 10000000000LL);
    QVERIFY(rv.label == QString("Wikipedia Example"));

    uri.setUrl(QString("litecoinz:L1SdUmxUpkrnRG5hBsdtXzjonYtQ8sjcSYG?message=Wikipedia Example Address"));
    QVERIFY(GUIUtil::parseBitcoinURI(uri, &rv));
    QVERIFY(rv.address == QString("L1SdUmxUpkrnRG5hBsdtXzjonYtQ8sjcSYG"));
    QVERIFY(rv.label == QString());

    QVERIFY(GUIUtil::parseBitcoinURI("litecoinz://L1SdUmxUpkrnRG5hBsdtXzjonYtQ8sjcSYG?message=Wikipedia Example Address", &rv));
    QVERIFY(rv.address == QString("L1SdUmxUpkrnRG5hBsdtXzjonYtQ8sjcSYG"));
    QVERIFY(rv.label == QString());

    uri.setUrl(QString("litecoinz:L1SdUmxUpkrnRG5hBsdtXzjonYtQ8sjcSYG?req-message=Wikipedia Example Address"));
    QVERIFY(GUIUtil::parseBitcoinURI(uri, &rv));

    uri.setUrl(QString("litecoinz:L1SdUmxUpkrnRG5hBsdtXzjonYtQ8sjcSYG?amount=1,000&label=Wikipedia Example"));
    QVERIFY(!GUIUtil::parseBitcoinURI(uri, &rv));

    uri.setUrl(QString("litecoinz:L1SdUmxUpkrnRG5hBsdtXzjonYtQ8sjcSYG?amount=1,000.0&label=Wikipedia Example"));
    QVERIFY(!GUIUtil::parseBitcoinURI(uri, &rv));
}

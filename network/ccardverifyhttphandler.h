#ifndef CCARDVERIFYHTTPHANDLER_H
#define CCARDVERIFYHTTPHANDLER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include <QDebug>
#include <QUrl>
#include "utility/json.h"
#include "device/ccandatapacker.h"
#include "utility/aeshandler.h"

class CCardVerifyHttpHandler : public QObject
{
    Q_OBJECT
public:
    explicit CCardVerifyHttpHandler(QObject *parent = 0);

    void    init();

    void    request(const QByteArray& data, const QString& connectorID, const QString &cardNo);

private:
    QNetworkAccessManager*  m_manager;

    QNetworkReply*          m_reply;

    QTimer*                 m_timer;

    bool                    m_bRet;

    QString                 httpUrl;

    QString                 connectorID;

    QString                 cardNo;
signals:
    /*
         *
        */
    void    sendRetMsg(const QByteArray& data);

    void    sendCardRetMsg(int status, QString sn, int sitecode, QString accountNo, int amount);

public slots:
    /*
             *
            */
    void    reqReply();

    /*
                 *
                */
    void    reqTimeout();
};

#endif // CCARDVERIFYHTTPHANDLER_H

#ifndef CBOOKORDERUPDATEHTTP_H
#define CBOOKORDERUPDATEHTTP_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include <QDebug>
#include <QUrl>
#include "utility/json.h"
#include "device/ccandatapacker.h"
#include "utility/aeshandler.h"

class CBookOrderUpdateHttp : public QObject
{
    Q_OBJECT
public:
    explicit CBookOrderUpdateHttp(QObject *parent = 0);

    void    init();

    void    request(const QByteArray& data);

private:
    QNetworkAccessManager*  m_manager;

    QNetworkReply*          m_reply;

    QTimer*                 m_timer;

    bool                    m_bRet;

    QString                 httpUrl;

    QString                 connectorID;

    QString                 cardNo;
signals:

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

#endif // CBOOKORDERUPDATEHTTP_H

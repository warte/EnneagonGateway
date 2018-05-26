#ifndef CALIREFUND_H
#define CALIREFUND_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include <QDebug>
#include <QMap>
#include "utility/json.h"
#include "utility/aeshandler.h"

class CAliRefund : public QObject
{
    Q_OBJECT
public:
    explicit CAliRefund(QObject *parent = 0);
    void    init();

    void    request(const QByteArray& data);

private:
    QNetworkAccessManager*  m_manager;

    QNetworkReply*          m_reply;

    QByteArray              mData;

    QTimer*                 m_timer;

    QString                 m_connectorID;

    QString                 m_orderNo;

    bool                    m_flowRet;

public slots:
    /*
     *
     */
    void    reqReply();

    void    reqTimeout();

};

#endif // CALIREFUND_H

#ifndef CFLOWNOHTTP_H
#define CFLOWNOHTTP_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include <QDebug>
#include <QMap>
#include "utility/json.h"
#include "utility/aeshandler.h"

/*
 * 请求流水号充电，如果在1分钟逗收不到服务器返回的流水号信息，
 * 那么本地生成流水号。本地记录相关信息。
 * 生成订单。
*/

class CFlowNoHttp : public QObject
{
    Q_OBJECT
public:
    explicit CFlowNoHttp(QObject *parent = 0);

    void    init();

    void    request(const QString connectorID, const QString flow, const QByteArray& data);

private:
    QNetworkAccessManager*  m_manager;

    QNetworkReply*          m_reply;

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

#endif // CFLOWNOHTTP_H

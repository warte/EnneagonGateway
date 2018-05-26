#ifndef CWECHATREFUND_H
#define CWECHATREFUND_H

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
class CWeChatRefund : public QObject
{
    Q_OBJECT
public:
    explicit CWeChatRefund(QObject *parent = 0);

    void    init();

    void    request(const QByteArray& data);

private:
    QNetworkAccessManager*  m_manager;

    QNetworkReply*          m_reply;

    QTimer*                 m_timer;

    QByteArray              mData;

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

#endif // CWECHATREFUND_H

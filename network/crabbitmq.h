#ifndef CRABBITMQ_H
#define CRABBITMQ_H

#include <QObject>
#include <qamqp/qamqpclient.h>
#include <qamqp/qamqpexchange.h>
#include <qamqp/qamqpmessage.h>
#include <qamqp/qamqpqueue.h>
#include <qamqp/qamqptable.h>
#include "manage/capp.h"
#include "utility/aeshandler.h"
#include "pile/cconnectormanager.h"

class CRabbitMQ : public QObject
{
    Q_OBJECT
public:
    explicit CRabbitMQ(QObject *parent = 0);

    //void    sendMsg(const QByteArray &msg, const QString &msgtype);

    void    sendStatusMsg(const QByteArray &msg);

    void    sendHeartRetMsg(const QByteArray &msg);

    void    sendBookRetMsg(const QByteArray &msg);

    void    sendRemoteRetMsg(const QByteArray &msg);

    void    resetMQ();

    void    initRemote();

    void    initRemoteRet();

    void    initBooking();

    void    initBookingRet();

    void    initHeartBeat();

    void    initHeartRet();

    void    initStatus();


    void    adjustdate(int year,int mon,int mday,int hour,int min,int sec);

signals:

public slots:
    void    start();

    void    clientConnected();

    void    disConnectFromServer();

    /*
     * 远程控制(启动停止)
     */
    void    remote_exchange_declared();
    void    remote_queue_declared();
    void    remote_queue_bound();
    void    remote_Msg_received();

    /*
     * remote return excahnge
    */
    void    remoteRet_exchange_declared();
    void    remoteRet_queue_declared();
    void    remoteRet_queue_bound();

    /*
     * 预约
    */
    void    booking_exchange_declared();
    void    booking_queue_decalred();
    void    booking_queue_bond();
    void    booking_Msg_received();

    /*
     *  Booking return exchange
    */
    void    bookRet_exchange_declared();
    void    bookRet_queue_decalred();
    void    bookRet_queue_bond();

    /*
     * 心跳消息
     */
    void    heart_exchange_declared();
    void    heartbeat_queue_declared();
    void    heartbeat_queue_bound();
    void    heartbeat_Msg_received();

    /*
     * heartbeat return exchange
    */
    void    heartRet_exchange_declared();
    void    heartRet_queue_declared();
    void    heartRet_queue_bound();

    /*
     * charger status ecahnge
    */
    void    status_exchange_declared();
    void    status_queue_declared();
    void    status_queue_bond();

    void    sockErr(QAbstractSocket::SocketError err);

    void    countTimeout();

private:
    QAmqpClient *m_client;

    QTimer      *mTimer;

    int         m_dLostCounts;

    bool        m_bConnected;

    bool        m_bSyncTime;

    QAmqpExchange   *m_remoteEx;
    QAmqpQueue      *m_remoteQue;

    QAmqpExchange   *m_bookingEx;
    QAmqpQueue      *m_bookingQue;

    QAmqpExchange   *m_heartbeatEx;
    QAmqpQueue      *m_heartbeatQue;

    QAmqpExchange   *m_remoteRetEx;
    QAmqpQueue      *m_remoteRetQue;

    QAmqpExchange   *m_bookRetEx;
    QAmqpQueue      *m_bookRetQue;

    QAmqpExchange   *m_heartRetEx;
    QAmqpQueue      *m_heartRetQue;

    QAmqpExchange   *m_statusEx;
    QAmqpQueue      *m_statusQue;
};

#endif // CRABBITMQ_H

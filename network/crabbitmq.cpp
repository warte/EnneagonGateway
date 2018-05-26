#include "crabbitmq.h"
#include "manage/capp.h"
#include "manage/cmanage.h"
#include <QHostAddress>

CRabbitMQ::CRabbitMQ(QObject *parent) : QObject(parent)
{
    m_bConnected = false;
    m_dLostCounts = 0;
    m_bSyncTime = false;

    mTimer = new QTimer(this);
    connect(mTimer, SIGNAL(timeout()), this, SLOT(countTimeout()));
    mTimer->start(1000);
}

void CRabbitMQ::countTimeout()
{
    m_dLostCounts += 1;
    if(m_dLostCounts > 60)
    {
        m_dLostCounts = 0;
        m_bConnected = false;
        qDebug() << "timeout reset mq";
        resetMQ();
    }
}

void CRabbitMQ::start()
{
    m_client = new QAmqpClient(this);

    m_client->setUsername(CApp::getInstance()->getSMqUser());
    m_client->setPassword(CApp::getInstance()->getSMqPassword());
    //m_client->setAutoReconnect(true, 1000);


    connect(m_client, SIGNAL(connected()), this, SLOT(clientConnected()));
    connect(m_client, SIGNAL(disconnected()), this, SLOT(disConnectFromServer()));
    connect(m_client, SIGNAL(socketError(QAbstractSocket::SocketError)), this, SLOT(sockErr(QAbstractSocket::SocketError)));

    m_client->connectToHost(QHostAddress(CApp::getInstance()->getSMqUrl()), CApp::getInstance()->getSMqPort().toInt());
}

void CRabbitMQ::disConnectFromServer()
{
    m_bConnected = false;
    qDebug() << "disconnect from server";
    qDebug() << "disconnect from server manual disconnect";
}

void CRabbitMQ::resetMQ()
{
    qDebug() << "reset mq";

    CConnectorManager::getInstance()->setBOnline(false);

    CManage::getInstance()->resetMq();
}

void CRabbitMQ::sockErr(QAbstractSocket::SocketError err)
{
    m_bConnected = false;
    qDebug() << "socket error reset mq" << err;
}

void CRabbitMQ::clientConnected()
{
    qDebug() << "rabbitmq connect to server. success";
    m_bConnected = true;
    m_dLostCounts = 0;

    initRemote();
    initRemoteRet();
    initBookingRet();
    initBooking();
    initHeartRet();
    initHeartBeat();
    initStatus();

    //CConnectorManager::getInstance()->setBOnline(true);
}

void CRabbitMQ::sendBookRetMsg(const QByteArray &msg)
{
    if((m_client) && (m_bookRetEx) && (m_client->isConnected()) && m_bConnected)
    {
        m_bookRetEx->publish(msg, "bookChg");
    }
}

void CRabbitMQ::sendHeartRetMsg(const QByteArray &msg)
{
    if((m_client) && (m_heartRetEx) && (m_client->isConnected()) && (m_bConnected))
    {
        m_heartRetEx->publish(msg, "hbr");
    }
}

void CRabbitMQ::sendRemoteRetMsg(const QByteArray &msg)
{
    if((m_client) && (m_remoteRetEx) && (m_client->isConnected()) && m_bConnected)
    {
        m_remoteRetEx->publish(msg, "remChgR");
    }
}

void CRabbitMQ::sendStatusMsg(const QByteArray &msg)
{
    if((m_client != NULL) && (m_statusEx != NULL) && (m_client->isConnected()) && m_bConnected)
    {
        m_statusEx->publish(msg, "charger");
    }
}

void CRabbitMQ::initRemote()
{
    m_remoteEx = m_client->createExchange("remChgEx");
    connect(m_remoteEx, SIGNAL(declared()), this, SLOT(remote_exchange_declared()));
    m_remoteEx->declare(QAmqpExchange::FanOut);
}

void CRabbitMQ::remote_exchange_declared()
{
    m_remoteQue =  m_client->createQueue();
    connect(m_remoteQue, SIGNAL(declared()), this, SLOT(remote_queue_declared()));
    connect(m_remoteQue, SIGNAL(bound()), this, SLOT(remote_queue_bound()));
    connect(m_remoteQue, SIGNAL(messageReceived()), this, SLOT(remote_Msg_received()));

    m_remoteQue->declare(QAmqpQueue::Exclusive);
}

void CRabbitMQ::remote_queue_declared()
{
    m_remoteQue->bind("remChgEx", "remChg");
}

void CRabbitMQ::remote_queue_bound()
{
    m_remoteQue->consume(QAmqpQueue::coNoAck);
}

void CRabbitMQ::remote_Msg_received()
{
    QAmqpMessage m_message = m_remoteQue->dequeue();
    qDebug() << m_message.payload();

    bool bOk;
    int temp;

    QByteArray pushdata = m_message.payload();
    AESHandler handler;
    std::string retstr = handler.CBC_AESDecryptStr(CApp::getInstance()->getKey(), CApp::getInstance()->getIv(), pushdata.constData());

    QByteArray data = QByteArray(retstr.c_str(), retstr.length());

    qDebug() << "remote command " << data;

    QtJson::JsonObject res = QtJson::parse(data,bOk).toMap();

    /*
     * remote start/stop charge .
     * param :
     * connectorCode
     * userChargeType
     * voltage
     * current
     * chargingMode
     * modeValue
     * dictate
     * paymentAccount
     * currTime
     * orderNo
     * mobileNumber
    */

    QString connectorCode = res["connectorCode"].toString();
    int userChargeType = res["userChargeType"].toInt();
    float voltage = res["voltage"].toFloat();
    float current = res["current"].toFloat();
    int chargingMode = res["chargingMode"].toInt();
    int dictate = res["dictate"].toInt();
    int value = 0;
    QString orderNo = res["orderNo"].toString();
    QString phoneNo = res["mobileNumber"].toString();

    //userChargeType  chargingMode
    if((userChargeType == 3) || (userChargeType  == 7) || (userChargeType == 8) || (userChargeType == 9))
    {
        if((chargingMode == 2) || (chargingMode == 3) )
        {

            if(res.contains("modeValue"))
            {
                value = (int)(res["modeValue"].toFloat() * 100);
            }

        }

    }else{

        if(res.contains("modeValue"))
        {
            value = res["modeValue"].toInt();
        }
    }

    QString paymentAccount = res["paymentAccount"].toString();

    CConnectorManager::getInstance()->handleRemoteControlCommand(connectorCode, userChargeType, voltage, current, chargingMode, value, paymentAccount, dictate);

    if((userChargeType == 11) && (dictate == 1)){
        CConnectorManager::getInstance()->setRemoteDetail(connectorCode, orderNo, phoneNo);
    }
}

void CRabbitMQ::initRemoteRet()
{
    m_remoteRetEx = m_client->createExchange("remChgREx");
    connect(m_remoteRetEx, SIGNAL(declared()), this, SLOT(remoteRet_exchange_declared()));
    m_remoteRetEx->declare(QAmqpExchange::Direct);
}

void CRabbitMQ::remoteRet_exchange_declared()
{
    m_remoteRetQue = m_client->createQueue("remChgRQue");
    connect(m_remoteRetQue, SIGNAL(declared()), this, SLOT(remoteRet_queue_declared()));
    connect(m_remoteRetQue, SIGNAL(bound()), this, SLOT(remoteRet_queue_bound()));
    //m_remoteRetQue->declare(QAmqpQueue::Durable);
}

void CRabbitMQ::remoteRet_queue_declared()
{
    m_remoteRetQue->bind("remChgREx", "remChgR");
}

void CRabbitMQ::remoteRet_queue_bound()
{
    m_remoteRetQue->consume(QAmqpQueue::coNoAck);
}

void CRabbitMQ::initBookingRet()
{
    m_bookRetEx = m_client->createExchange("bookChgREx");
    connect(m_bookRetEx, SIGNAL(declared()), this, SLOT(bookRet_exchange_declared()));
    m_bookRetEx->declare(QAmqpExchange::Direct);
}

void CRabbitMQ::bookRet_exchange_declared()
{
    m_bookRetQue = m_client->createQueue("bookChgRQue");
    connect(m_bookRetQue, SIGNAL(declared()), this, SLOT(bookRet_queue_decalred()));
    connect(m_bookRetQue, SIGNAL(bound()), this, SLOT(bookRet_queue_bond()));
    //m_bookRetQue->declare(QAmqpQueue::Durable);
}

void CRabbitMQ::bookRet_queue_decalred()
{
    m_bookRetQue->bind("bookChgREx", "bookChgR");
}

void CRabbitMQ::bookRet_queue_bond()
{
    m_bookRetQue->consume(QAmqpQueue::coNoAck);
}

void CRabbitMQ::initBooking()
{
    m_bookingEx = m_client->createExchange("bookChgEx");
    connect(m_bookingEx, SIGNAL(declared()), this, SLOT(booking_exchange_declared()));
    m_bookingEx->declare(QAmqpExchange::FanOut);
}

void CRabbitMQ::booking_exchange_declared()
{
    m_bookingQue =  m_client->createQueue();
    connect(m_bookingQue, SIGNAL(declared()), this, SLOT(booking_queue_decalred()));
    connect(m_bookingQue, SIGNAL(bound()), this, SLOT(booking_queue_bond()));
    connect(m_bookingQue, SIGNAL(messageReceived()), this, SLOT(booking_Msg_received()));

    m_bookingQue->declare(QAmqpQueue::Exclusive);
}

void CRabbitMQ::booking_queue_decalred()
{
    m_bookingQue->bind("bookChgEx", "bookChg");
}

void CRabbitMQ::booking_queue_bond()
{
    m_bookingQue->consume(QAmqpQueue::coNoAck);
}

void CRabbitMQ::booking_Msg_received()
{
    QAmqpMessage m_message = m_bookingQue->dequeue();
    //qDebug() << m_message.payload();

    bool bOk;
    int temp;

    QByteArray pushdata = m_message.payload();
    AESHandler handler;
    std::string retstr = handler.CBC_AESDecryptStr(CApp::getInstance()->getKey(), CApp::getInstance()->getIv(), pushdata.constData());

    QByteArray data = QByteArray(retstr.c_str(), retstr.length());

    qDebug() << "booking command " << data;

    QtJson::JsonObject res = QtJson::parse(QByteArray(retstr.c_str(), retstr.length()),bOk).toMap();

    /*
     * remote start/stop charge .
     * param :
     * connectorCode
     * userChargeType
     * voltage
     * current
     * chargingMode
     * modeValue
     * dictate
     * paymentAccount
     * currTime
     * overTime 超时时间
    */

    QString connectorCode = res["connectorCode"].toString();
    int userChargeType = res["userChargeType"].toInt();
    float voltage = res["voltage"].toFloat();
    float current = res["current"].toFloat();
    QString bookingDate = res["bookingDate"].toString();
    int chargingMode = res["chargingMode"].toInt();
    int dictate = res["dictate"].toInt();
    QString plate = res["plate"].toString();
    QString overTime = res["overTime"].toString();

    int value = 0;
    if(res.contains("modeValue"))
    {
        value = res["modeValue"].toInt();
    }
    QString paymentAccount = res["paymentAccount"].toString();

    CConnectorManager::getInstance()->handleRemoteBookingCommand(connectorCode, userChargeType, bookingDate, voltage, current, chargingMode, value, paymentAccount, dictate, plate, overTime);
}

void CRabbitMQ::initHeartRet()
{
    m_heartRetEx = m_client->createExchange("hbrEx");
    connect(m_heartRetEx, SIGNAL(declared()), this, SLOT(heartRet_exchange_declared()));
    m_heartRetEx->declare(QAmqpExchange::Direct);
}

void CRabbitMQ::heartRet_exchange_declared()
{
    m_heartRetQue = m_client->createQueue("hbrQue");
    connect(m_heartRetQue, SIGNAL(declared()), this, SLOT(heartRet_queue_declared()));
    connect(m_heartRetQue, SIGNAL(bound()), this, SLOT(heartRet_queue_bound()));
    //m_heartRetQue->declare(QAmqpQueue::AutoDelete);
}

void CRabbitMQ::heartRet_queue_declared()
{
    m_heartRetQue->bind("hbrEx", "hbr");
}

void CRabbitMQ::heartRet_queue_bound()
{
    m_heartRetQue->consume(QAmqpQueue::coNoAck);
}

void CRabbitMQ::initHeartBeat()
{
    m_heartbeatEx = m_client->createExchange("hbEx");
    connect(m_heartbeatEx, SIGNAL(declared()), this, SLOT(heart_exchange_declared()));
    m_heartbeatEx->declare(QAmqpExchange::FanOut);
}

void CRabbitMQ::heart_exchange_declared()
{
    m_heartbeatQue = m_client->createQueue();
    connect(m_heartbeatQue, SIGNAL(declared()), this, SLOT(heartbeat_queue_declared()));
    connect(m_heartbeatQue, SIGNAL(bound()), this, SLOT(heartbeat_queue_bound()));
    connect(m_heartbeatQue, SIGNAL(messageReceived()), this, SLOT(heartbeat_Msg_received()));

    m_heartbeatQue->declare(QAmqpQueue::Exclusive);
}

void CRabbitMQ::heartbeat_Msg_received()
{
    //CManage::getInstance()->handleHeartBeat(true);
    QAmqpMessage m_message = m_heartbeatQue->dequeue();
    QByteArray pushdata = m_message.payload();
    m_dLostCounts = 0;

    qDebug() << "enneagon mq" << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << m_message.payload();

    bool bOk;
    AESHandler handler;
    std::string retstr = handler.CBC_AESDecryptStr(CApp::getInstance()->getKey(), CApp::getInstance()->getIv(), pushdata.constData());

    QtJson::JsonObject res = QtJson::parse(QByteArray(retstr.c_str(), retstr.length()),bOk).toMap();

    QString timestamp = res["currTime"].toString();


    QDateTime cur = QDateTime::fromString(timestamp, "yyyy-MM-dd hh:mm:ss");

    if(m_bSyncTime == false)
    {
        adjustdate(cur.date().year(), cur.date().month(), cur.date().day(),
                   cur.time().hour(), cur.time().minute(), cur.time().second());

        m_bSyncTime = true;
    }

    CConnectorManager::getInstance()->syncChargerTime(timestamp);

    CConnectorManager::getInstance()->setBOnline(true);

    CConnectorManager::getInstance()->pushHeartBeatMsg();
}

void CRabbitMQ::adjustdate(int year,int mon,int mday,int hour,int min,int sec)
{
    time_t t;
    struct tm nowtime;
    nowtime.tm_sec=sec;/* Seconds.[0-60] (1leapsecond)*/
    nowtime.tm_min=min;/* Minutes.[0-59]  */
    nowtime.tm_hour=hour;/* Hours. [0-23]*/
    nowtime.tm_mday=mday;/* Day.[1-31] */
    nowtime.tm_mon=mon-1;/* Month. [0-11]*/
    nowtime.tm_year=year-1900 ;/* Year-1900.*/
    nowtime.tm_isdst=-1 ;/* DST.[-1/0/1]*/
    t=mktime(&nowtime);
    stime(&t);
}

void CRabbitMQ::heartbeat_queue_declared()
{
    m_heartbeatQue->bind("hbEx", "hb");
}

void CRabbitMQ::heartbeat_queue_bound()
{
    m_heartbeatQue->consume(QAmqpQueue::coNoAck);
}

void CRabbitMQ::initStatus()
{
    m_statusEx = m_client->createExchange("chargerEx");
    connect(m_statusEx, SIGNAL(declared()), this, SLOT(status_exchange_declared()));
    m_statusEx->declare(QAmqpExchange::Direct);
}

void CRabbitMQ::status_exchange_declared()
{
    m_statusQue = m_client->createQueue("chargerQue");
    connect(m_statusQue, SIGNAL(declared()), this, SLOT(status_queue_declared()));
    connect(m_statusQue, SIGNAL(bound()), this, SLOT(status_queue_bond()));
    //m_statusQue->declare(QAmqpQueue::AutoDelete);
}

void CRabbitMQ::status_queue_declared()
{
    m_statusQue->bind("chargerEx", "charger");
}

void CRabbitMQ::status_queue_bond()
{
    m_statusQue->consume(QAmqpQueue::coNoAck);
}

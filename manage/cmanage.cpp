#include "cmanage.h"

CManage* CManage::mInstance = NULL;

CManage::CManage(QObject *parent) : QObject(parent)
{
}

CManage* CManage::getInstance()
{
    if(!mInstance)
    {
        mInstance = new CManage();
        /*
         * 读取配置
        */
        mInstance->readDefaultSetting();
        /*
         * 初始化设备
        */
        mInstance->initDevice();
        /*
         * 初始化网络
        */
        mInstance->initNetwork();
        /*
         * 初始化信号槽
        */
        mInstance->initConnection();
        /*
         * 注册
        */
        mInstance->registeSystem();
        /*
         * 开启系统
        */
        mInstance->startSystem();
        /*
         * 初始化定时器
        */
        mInstance->initTimer();
        /*
         *
        */
        CPileDataHelper::getInstance()->getName();

        /*
         * 更新历史报文接口
        */
        CDataUploader::getInstance()->init();
    }

    return mInstance;
}

void CManage::initDevice()
{
    m_canWrapper = new CanWrapper();
    m_canThread = new CanWorkerThread();

    m_canWrapper1 = new CanWrapper();//CAN1
    m_canThread1 = new CanWorkerThread();//CAN1

}

void CManage::closeDevice()
{
    m_canThread->shutDown();
    m_canWrapper->Close();

    m_canThread1->shutDown();//CAN1
    m_canWrapper1->Close();//CAN1

    if(!m_canThread->wait(3000))
    {
        m_canThread->terminate();
    }

    if(!m_canThread1->wait(3000))//CAN1
    {
        m_canThread1->terminate();//CAN1
    }

    delete m_canThread;
    delete m_canWrapper;

    delete m_canThread1;//CAN1
    delete m_canWrapper1;//CAN1

}


void CManage::initNetwork()
{
    m_rabbitMQ = new CRabbitMQ(this);
    m_rabbitMQ->start();
    //m_sock = new CRemoteSocket(this);
    //m_sock->init();
}

void CManage::updateMQConnectionInfo(QString &host, QString port, QString &user, QString &pwd)
{
    QMutexLocker locker(&mutex);
    if(CApp::getInstance()->isMqStandSame(host, port, user, pwd) == false)
    {
        CPileDataHelper::getInstance()->updateMqInfo(host, port, user, pwd);
        // 更新句柄
        delete m_rabbitMQ;
        m_rabbitMQ = NULL;
        m_rabbitMQ = new CRabbitMQ(this);
        m_rabbitMQ->start();
    }

}

void CManage::resetMq()
{
    QMutexLocker locker(&mutex);
    delete m_rabbitMQ;
    m_rabbitMQ = NULL;
    m_rabbitMQ = new CRabbitMQ(this);
    m_rabbitMQ->start();

}

void CManage::closeNetwork()
{
    delete m_rabbitMQ;
    m_rabbitMQ = NULL;
}

void CManage::registeSystem()
{
    CConnectorManager::getInstance()->registeSystem();
}

void CManage::startSystem()
{
    int ret  = 0;

    m_canWrapper->Init("can0", ret);
    m_canWrapper1->Init("can1", ret);//CAN1

    m_canWrapper->EnableErrorMessages();
    m_canWrapper1->EnableErrorMessages();//CAN1

    m_canThread->Init(m_canWrapper);
    m_canThread1->Init(m_canWrapper1);//CAN1

    m_canThread->start();
    m_canThread1->start();//CAN1

    qDebug() << "start system";

}

void CManage::sendMsgToPile(const QByteArray& data)
{
    m_canThread->sendReplyData(data);
    m_canThread1->sendReplyData(data);//CAN1

}

void CManage::sendFlowNoRequest(QString connectorID, const QString& localFlow, const QByteArray& reqdata)
{
    CFlowNoHttp* reqHttp = new CFlowNoHttp();
    reqHttp->request(connectorID, localFlow, reqdata);
}

void CManage::sendBookOrderCreateMsg(const QByteArray &data)
{
    CBookOrderHttp* book = new CBookOrderHttp();
    book->request(data);
}

void CManage::sendAliRefund(const QByteArray &data)
{
    CAliRefund* refund = new CAliRefund();
    refund->request(data);
}

void CManage::sendWechatRefund(const QByteArray &data)
{
    CWeChatRefund* refund = new CWeChatRefund();
    refund->request(data);
}

void CManage::sendBookOrderUpdateMsg(const QByteArray &data)
{
    CBookOrderUpdateHttp* update = new CBookOrderUpdateHttp();
    update->request(data);
}

void CManage::sendCardVerifyToNetwork(const QByteArray& data, const QString& connectotID, const QString &cardNO)
{
    CCardVerifyHttpHandler *verify = new CCardVerifyHttpHandler();
    verify->request(data, connectotID, cardNO);
}

void CManage::sendChargerStateChangeToNetwork(const QByteArray &data, const QString &connectorID)
{
    CStateHttp *state = new CStateHttp();
    state->request(data);
}

void CManage::sendRegisteReqMsgToNetwork(const QByteArray& data)
{
    CRegHttp* reg = new CRegHttp(this);
    reg->request(data);
}

void CManage::sendConsumeMsgToNetwork(const QString &connectorID, const QString &flow, const QByteArray &data)
{
    CConsumeHttp* consume = new CConsumeHttp(this);
    consume->request(connectorID, flow, data);
}

void CManage::sendRecordMsgToNetwork(const QString &connectorID, const QString &flow, const QByteArray &data)
{
    CRecordHttp* rec = new CRecordHttp(this);
    rec->request(connectorID, flow, data);
}

void CManage::sendMsgToMQ(const QByteArray &msg, const QString &type)
{
    //QMutexLocker locker(&mutex);
    if(type.compare("remoteRet") == 0)
    {
        if(m_rabbitMQ != NULL)
        {
            m_rabbitMQ->sendRemoteRetMsg(msg);
        }

    }
    else if (type.compare("bookRet") == 0)
    {
        if(m_rabbitMQ != NULL)
        {
            m_rabbitMQ->sendBookRetMsg(msg);
        }

    }
    else if(type.compare("heartbeatRet") == 0)
    {
        if(m_rabbitMQ != NULL)
        {
            m_rabbitMQ->sendHeartRetMsg(msg);
        }

    }
    else if(type.compare("charging") == 0)
    {
        if(m_rabbitMQ != NULL)
        {
            m_rabbitMQ->sendStatusMsg(msg);
        }

    }
    else if(type.compare("statechange") == 0)
    {
        if(m_rabbitMQ != NULL)
        {
            m_rabbitMQ->sendStatusMsg(msg);
        }
    }
}

void CManage::initConnection()
{
    /*
     * 处理桩上报的报文
    */
    connect(m_canThread, SIGNAL(sendCanMsg(QByteArray)), this, SLOT(setData(QByteArray)));
    connect(m_canThread1, SIGNAL(sendCanMsg(QByteArray)), this, SLOT(setData(QByteArray)));//CAN1

    //connect(m_canThread1, SIGNAL(sendCanMsg(QByteArray)), this, SLOT(setData(QByteArray)));
}

void CManage::initTimer()
{
    m_regTimer = new QTimer(this);
    connect(m_regTimer, SIGNAL(timeout()), this, SLOT(keepRegTimeout()));
    m_regTimer->start(CApp::getInstance()->getDDeltaTime());
}

/*
 * 定时发送状态到平台
*/
void CManage::keepRegTimeout()
{
    if(CConnectorManager::getInstance()->getRegisteStatus() == false)
    {
        CConnectorManager::getInstance()->registeSystem();
    }
    else
    {
        m_regTimer->stop();
    }
}

/*
 * 将从CAN网络上收到的数据分发到各个桩上进行数据处理
*/
void CManage::setData(QByteArray data)
{
    CConnectorManager::getInstance()->handleCanMsg(data);
}

void CManage::readDefaultSetting()
{
    CApp::getInstance()->init();
    CConnectorManager::getInstance()->init();
}


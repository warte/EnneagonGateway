#include "cconnectormanager.h"
#include "manage/cmanage.h"
#include <unistd.h>

CConnectorManager* CConnectorManager::mInstance = NULL;

CConnectorManager::CConnectorManager(QObject *parent) : QObject(parent)
{
    m_bOnline = false;
    m_bGetMsg = false;
    currIndex = 0;
}

CConnectorManager* CConnectorManager::getInstance()
{
    if(!mInstance)
    {
        mInstance = new CConnectorManager();
    }

    return mInstance;
}

bool CConnectorManager::getBOnline() const
{
    return m_bOnline;
}

void CConnectorManager::setBOnline(bool bOnline)
{
    if((m_bOnline == false) && (bOnline == true))
    {
        m_bOnline = bOnline;
        for(int i = 0; i < m_connectorList.size(); i++)
        {
            m_connectorList.at(i)->updateStatus();
        }
    }

    m_bOnline = bOnline;
    if(bOnline)
    {
        m_bGetMsg = true;
    }
}

QList<CEnneagonConnector *> CConnectorManager::getConnectorList() const
{
    return m_connectorList;
}

void CConnectorManager::init()
{
    CPileDataHelper::getInstance()->queryPile();

    CPileDataHelper::getInstance()->queryWarning("1");

    CPileDataHelper::getInstance()->queryWarning("2");

    mTimer = new QTimer(this);
    connect(mTimer, SIGNAL(timeout()), this, SLOT(handleTimeout()));
    mTimer->start(CApp::getInstance()->getDDeltaTime());
}

/*
 *  sending heartbeat msg to pile
 *  sending msg to server
*/
void CConnectorManager::handleTimeout()
{
    QString sn = m_connectorList.at(currIndex)->sChargerSn();
    for(int i = 0; i < m_connectorList.size(); i++)
    {
        if(m_connectorList.at(i)->sChargerSn().compare(sn) == 0)
        {
            CCANDataPakcer packer;
            packer.setDevID(m_connectorList.at(i)->dDevID());
            if(m_bOnline)
            {
                CManage::getInstance()->sendMsgToPile(packer.packHeartbeatMsg(1));
            }
            else
            {
                CManage::getInstance()->sendMsgToPile(packer.packHeartbeatMsg(2));
            }

            usleep(100 * 1000);

            currIndex += 1;

            if(m_connectorList.at(i)->status().getChargerState() == 2)
            {
                // query the charging status
                CCANDataPakcer packer;
                packer.setDevID(m_connectorList.at(i)->dDevID());
                CManage::getInstance()->sendMsgToPile(packer.packQueryStatusMsg(m_connectorList.at(i)->order().getConnectorFlowNo()));
            }
        }

        //
        m_connectorList.at(i)->updateCounts();
    }

    if(currIndex >= (m_connectorList.size()))
    {
        currIndex = 0;
    }

    if(m_bGetMsg)
    {
        CManage::getInstance()->sendMsgToMQ(packHeartBeatMsg(), "heartbeatRet");
        m_bGetMsg = false;
    }
}

void CConnectorManager::pushHeartBeatMsg()
{
    //
    //m_bGetMsg = false;
}

QByteArray CConnectorManager::packHeartBeatMsg()
{
    QVariantMap param;
    param.insert("stationCode", CApp::getInstance()->getSStationID());
    param.insert("gatewayCode", CApp::getInstance()->getSGatewaySn());

    QVariantList offlines;
    QVariantList onlines;
    for(int i = 0; i < m_connectorList.size(); i++)
    {
        if(m_connectorList.at(i)->bConnectedToPile() && (!onlines.contains(m_connectorList.at(i)->sChargerSn())))
        {
            onlines.append(m_connectorList.at(i)->sChargerSn());
        }

        if(!m_connectorList.at(i)->bConnectedToPile() && (!offlines.contains(m_connectorList.at(i)->sChargerSn())))
        {
            offlines.append(m_connectorList.at(i)->sChargerSn());
        }
    }
    param.insert("onLineArr", onlines);
    param.insert("unonLineArr", offlines);

    QByteArray data = QtJson::serialize(param);
    AESHandler handler;

    return handler.encodeMsg(data);
}

/*
 * 读取数据库内容，初始化枪口信息
*/
void CConnectorManager::getSysPileMsg(QSqlRecord rec)
{
    int gun = rec.value(rec.indexOf("chargerGun")).toInt();
    bool bOk;
    for(int i = 0; i < gun; i++)
    {
        CEnneagonConnector* connector = new CEnneagonConnector;
        connector->setSChargerSn(rec.value(rec.indexOf("chargerSn")).toString());
        connector->setDGun(i);
        unsigned char t1 = 0x41 + i;
        connector->setSConnectorID(rec.value(rec.indexOf("chargerSn")).toString() + QString(t1));
        ushort devID = ((0xa0 | i) << 8) | rec.value(rec.indexOf("devID")).toString().toUShort(&bOk, 16);
        connector->setDDevID(devID);
        connector->setSSoftwareVersion(rec.value(rec.indexOf("softwareVersion")).toString());
        connector->setSHardwareVersion(rec.value(rec.indexOf("hardwareVersion")).toString());
        connector->setBIsReg(rec.value(rec.indexOf("isReg")).toBool());

        connector->showDetail();

        m_connectorList.push_back(connector);
    }
}

/*
 * 读取数据库内容，初始化告警信息
*/
void CConnectorManager::getSysWarning(QSqlRecord rec)
{
    for(int i = 0; i < m_connectorList.size(); i++)
    {
        m_connectorList.at(i)->status().getSysErrCode(rec);
    }
}

void CConnectorManager::registeSystem()
{
    for(int i = 0; i < m_connectorList.size(); i++)
    {
        if(!m_connectorList.at(i)->bIsReg() && (m_connectorList.at(i)->dGun() == 0))
        {
            CManage::getInstance()->sendRegisteReqMsgToNetwork(m_connectorList.at(i)->packRegisteMsg());
        }
    }
}

bool CConnectorManager::getRegisteStatus()
{
    bool bret = true;
    for(int i = 0; i < m_connectorList.size(); i++)
    {
        if(m_connectorList.at(i)->bIsReg() == false)
        {
            bret = false;
        }
    }

    return bret;
}

void CConnectorManager::syncChargerTime(QString timestamp)
{
    for(int i = 0; i < m_connectorList.size(); i++)
    {
        if((m_connectorList.at(i)->bSyncTime() == false) && (m_connectorList.at(i)->dGun() == 0))
        {
            CCANDataPakcer packer;
            packer.setDevID(m_connectorList.at(i)->dDevID());
            CManage::getInstance()->sendMsgToPile(packer.packSyncTimeMsg(timestamp));
        }
    }
}

QByteArray CConnectorManager::getChargingMsgByCode(const QString &connectorID)
{
    for(int i = 0; i < m_connectorList.size(); i++)
    {
        if(m_connectorList.at(i)->sConnectorID().compare(connectorID) == 0)
        {
            return m_connectorList.at(i)->packChargingMsg();
        }
    }
}

/*
 * handle remote command ,start/stop
*/
void CConnectorManager::handleRemoteControlCommand(QString connectorID, int userChargeType, float vol, float cur, int mode, int value, QString account, int order)
{

    for(int i = 0; i < m_connectorList.size(); i++)
    {
        if(m_connectorList.at(i)->sConnectorID().compare(connectorID) == 0)
        {
           m_connectorList.at(i)->takeControl(userChargeType, vol, cur, mode, value, account, order);

           m_connectorList.at(i)->setStartType(QString::number(userChargeType), account, "");
        }
    }
}

void CConnectorManager::setRemoteDetail(QString connectorID, QString orderNo, QString phone)
{
    for(int i = 0; i < m_connectorList.size(); i++){
        if(m_connectorList.at(i)->sConnectorID().compare(connectorID) == 0){
            m_connectorList.at(i)->setRemoteOrderDetail(orderNo, phone);
        }
    }
}

/*
 * handle remote booking command start booking /stop booking
*/
void CConnectorManager::handleRemoteBookingCommand(QString connectorID, int userChargeType, QString bookdate, float vol, float cur, int mode, int value, QString account, int order, QString plate, QString overtime)
{
    for(int i = 0; i < m_connectorList.size(); i++)
    {
        if(m_connectorList.at(i)->sConnectorID().compare(connectorID) == 0)
        {
            m_connectorList.at(i)->bookCharger(userChargeType, bookdate, vol, cur, mode,value, account, order, plate, overtime);
        }
    }
}

/*
 * 获取激活信息
*/
void CConnectorManager::getRegisteActivateMsg(QString sn, bool bactive)
{
    for(int i = 0; i < m_connectorList.size(); i++)
    {
        if(m_connectorList.at(i)->sChargerSn().compare(sn) == 0)
        {
            m_connectorList.at(i)->setBIsReg(bactive);
        }
    }
}

/*
 * 获取后台的订单创建状态
*/
void CConnectorManager::getPlatformFlowRetMsg(QString connectorID, bool bret)
{
    for(int i = 0; i < m_connectorList.size(); i++)
    {
        if(m_connectorList.at(i)->sConnectorID().compare(connectorID) == 0)
        {
            m_connectorList.at(i)->order().setBCreateSuccess(bret);
        }
    }
}

/*
 * send charging msg
*/
void CConnectorManager::sendChargingMsg(const QString &connectorcode)
{
    for(int i = 0; i < m_connectorList.size(); i++)
    {
        if(m_connectorList.at(i)->sConnectorID().compare(connectorcode) == 0)
        {
            CManage::getInstance()->sendMsgToMQ(m_connectorList.at(i)->packChargingMsg(), "charging");
        }
    }
}

/*
 * get platfrom consume msg ret
*/
void CConnectorManager::getPlatformConsumeRetMsg(QString connectorID, QString flowNo, bool bret)
{
    for(int i = 0; i < m_connectorList.size(); i++)
    {
        if((m_connectorList.at(i)->sConnectorID().compare(connectorID) == 0))
        {
            m_connectorList.at(i)->order().setBConsume(bret);
        }
    }
}

void CConnectorManager::getCardVerifyRetMsg(const QString &connectorID, int status,  const QString &cardNo)
{
    for(int i = 0; i < m_connectorList.size(); i++)
    {
        if(m_connectorList.at(i)->sConnectorID().compare(connectorID) == 0)
        {
            //if(status == 200)
            {
                m_connectorList.at(i)->setStartType("2", "", "4");
                m_connectorList.at(i)->setCardNo(cardNo);
            }
            /*
             * bug the charger recv the card error verify msg
             * but still charging.
            */

        }
    }
}

void CConnectorManager::setReadyToCompleteOrder(const QString &code)
{
    for(int i = 0; i < m_connectorList.size(); i++)
    {
        if(m_connectorList.at(i)->sConnectorID().compare(code) == 0)
        {
            m_connectorList.at(i)->readyToFinishOrder();
        }
    }
}

/*
 * 分发处理CAN报文
*/
void CConnectorManager::handleCanMsg(QByteArray data)
{
    ushort devid = (data[2] << 8) | data[3];
    ushort msgid = (data[0] << 8) | data[1];

    unsigned char temp[8];
    for(int j = 0; j < 8; j++)
    {
        temp[j] = data.at(4 + j);
    }

    for(int i = 0; i < m_connectorList.size(); i++)
    {
        if(m_connectorList.at(i)->dDevID() == devid)
        {
            m_connectorList.at(i)->decode(msgid, temp);
        }
    }
}

ushort CConnectorManager::getDevIDByConnetorID(const QString &connectorID)
{
    ushort ret = 0x0000;

    for(int i = 0; i < m_connectorList.size(); i++)
    {
        if(m_connectorList.at(i)->sConnectorID().compare(connectorID) == 0)
        {
            ret = m_connectorList.at(i)->dDevID();
        }
    }

    return ret;
}


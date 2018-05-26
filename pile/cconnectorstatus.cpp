#include "cconnectorstatus.h"
#include "manage/cmanage.h"
#include "manage/capp.h"

CConnectorStatus::CConnectorStatus()
{
    mRecv = 0;
    chargerState = 0;
    lostCounts = 0;
    connectorState = 0;
    bSocSet = false;
    m_dStartSoc = 0;
    resetParam();
}

void CConnectorStatus::resetParam()
{
    bSocSet = false;
    mRecv = 0;
    m_dStartSoc = 0;
    m_fVoltage = 0.0;
    m_fCurrent = 0.0;
    m_dStartSoc = 0;
    bSocSet = false;
    m_dEndSoc = 0;
    m_dCurrentSoc = 0;
    m_dConnectorTemp = 0;
    m_dRemainTime = 0;
    m_fAmount = 0.0;
    m_dChargingTime = 0;
    m_fQuantity = 0.0;
    m_fSharp = 0.0;
    m_fPeak = 0.0;
    m_fFlat = 0.0;
    m_fValley = 0.0;

    m_fBatteryRatedVoltage = 0.0;
    // 电池标称总能量
    m_fBatteryRatedPower = 0.0;
    // 最高允许充电总压
    m_fHighestAllowChargeVoltage = 0.0;
    // 最高允许充电电流
    m_fHighestAllowChargeCurrent = 0.0;
    // 单体截止电压
    m_fCellCutoffVoltage = 0.0;
    // 动力蓄电池开始soc
    m_dMotiveBatteryStartSoc = 0;
    // 电池最高允许温度
    m_dHighestAllowTemperature = 0;
    // 电压需求
    m_fBmsReqVoltage = 0.0;
    // 电流需求
    m_fBmsReqCurrent = 0.0;
    // 单体最高电压
    m_fCellHighestVoltage = 0.0;
    // 单体最高电压组号
    m_dCellHighestVoltageGroup = 0;
    // 电池最高温度
    m_dBatteryHighestTemperature = 0;
    // 电池最低温度
    m_dBatteryLowestTemperature = 0;

    // 单体最高电压所在编号
    m_dCellHighestVoltageNO = 0;
    // 最高温度监测点编号
    m_dHighestTempNo = 0;
    // 最低温度监测点编号
    m_dLowestTempNo = 0;

    /*
     * 交流进线信息
    */
    // A相交流电压
    m_fACVoltageA = 0.0;
    // B相交流电压
    m_fACVoltageB = 0.0;
    // C相交流电压
    m_fACVoltageC = 0.0;
    // A相交流电流
    m_fACCurrentA = 0.0;
    // B相交流电流
    m_fACCurrentB = 0.0;
    // C相交流电流
    m_fACCurrentC = 0.0;
}

void CConnectorStatus::setOfflineState()
{
    // 0 即离线状态
    chargerState = 0;
}

void CConnectorStatus::decodeCanMsg(int msgType, unsigned char *data)
{
    CCANDataPakcer packer;
    packer.setDevID(CConnectorManager::getInstance()->getDevIDByConnetorID(connectorCode));
    int temp = 0;
    switch (msgType) {
    case 0x0885:
        handleHeartBeatMsg(data);
        break;
    case 0x0840:
    case 0x0841:
    case 0x0843:
    case 0x0844:
        handleBMSMsg(msgType, data);
        break;
    case 0x0845:
        mRecv |= 0x10;
        m_fACVoltageA = (float)(((data[0] & 0xff) << 8) | (data[1] & 0xff)) / 100.0;
        m_fACVoltageB = (float)(((data[2] & 0xff) << 8) | (data[3] & 0xff)) / 100.0;
        m_fACVoltageC = (float)(((data[4] & 0xff) << 8) | (data[5] & 0xff)) / 100.0;
        break;
    case 0x0846:
        mRecv |= 0x20;
        m_fACCurrentA = (float)(((data[0] & 0xff) << 8) | (data[1] & 0xff)) / 100.0;
        m_fACCurrentB = (float)(((data[2] & 0xff) << 8) | (data[3] & 0xff)) / 100.0;
        m_fACCurrentC = (float)(((data[4] & 0xff) << 8) | (data[5] & 0xff)) / 100.0;
        break;
    case 0x0813:
        mRecv |= 0x40;
        m_fVoltage = (float)(((data[0] & 0xff) << 8) | (data[1] & 0xff)) / 10.0;
        m_fCurrent = (float)(((data[2] & 0xff) << 8) | (data[3] & 0xff)) / 10.0;
        m_dCurrentSoc = data[4] & 0xff;
        if((bSocSet == false) && (m_dCurrentSoc > 0))
        {
            m_dStartSoc = data[4] & 0xff;
            bSocSet = true;
        }
        m_dConnectorTemp = (data[5] & 0xff) - 50;
        m_dRemainTime = ((data[6] & 0xff) << 8) | (data[7] & 0xff);
        break;
    case 0x0812:
        mRecv |= 0x80;
        m_fAmount = (float)(((data[0] & 0xff) << 8) | (data[1] & 0xff)) / 100.0;
        m_dChargingTime = ((data[2] & 0xff) << 8) | (data[3] & 0xff);
        m_fQuantity = (float)(((data[4] & 0xff) << 24) | ((data[5] & 0xff) << 16) | ((data[6] & 0xff) << 8) | (data[7] & 0xff)) / 100.0;
        break;
    case 0x0817:
        mRecv |= 0x100;
        m_fSharp = (float)(((data[0] & 0xff) << 24) | ((data[1] & 0xff) << 16) | ((data[2] & 0xff) << 8) | (data[3] & 0xff)) / 100.0;
        m_fPeak = (float)(((data[4] & 0xff) << 24) | ((data[5] & 0xff) << 16) | ((data[6] & 0xff) << 8) | (data[7] & 0xff)) / 100.0;
        CManage::getInstance()->sendMsgToPile(packer.packReturnMsg(0x0817, true));
        break;
    case 0x0818:
        mRecv |= 0x200;
        m_fFlat = (float)(((data[0] & 0xff) << 24) | ((data[1] & 0xff) << 16) | ((data[2] & 0xff) << 8) | (data[3] & 0xff)) / 100.0;
        m_fValley = (float)(((data[4] & 0xff) << 24) | ((data[5] & 0xff) << 16) | ((data[6] & 0xff) << 8) | (data[7] & 0xff)) / 100.0;
        CManage::getInstance()->sendMsgToPile(packer.packReturnMsg(0x0818, true));
        break;
    default:
        break;
    }

    if((mRecv & 0x4ff) == 0x4ff)
    {
        mRecv = 0;
        CConnectorManager::getInstance()->sendChargingMsg(connectorCode);
    }
}

void CConnectorStatus::handleBMSMsg(int msgType, unsigned char *data)
{
    int temp = 0;
    short tempcur = 0;
    switch (msgType) {
    case 0x0840:
        mRecv |= 0x01;
        temp = data[0];
        if(temp = 0)
        {
            bmsProtocol = BMS_2011;
        }
        else if(temp == 1)
        {
            bmsProtocol = BMS_2015;
        }

        m_dBatteryType = (int)(data[1] & 0xff);
        m_fBatteryCapacity = (float)(((data[2] & 0xff) << 8) | (data[3] & 0xff)) / 10.0;
        m_fBatteryRatedVoltage = (float)(((data[4] & 0xff) << 8) | (data[5] & 0xff)) / 10.0;
        m_fBatteryRatedPower = (float)(((data[6] & 0xff) << 8) | (data[7] & 0xff)) / 10.0;
        break;
    case 0x0841:
        mRecv |= 0x02;
        m_fHighestAllowChargeVoltage = (float)(((data[0] & 0xff) << 8) | (data[1] & 0xff)) / 10.0;
        tempcur = ((data[2] & 0xff) << 8) | (data[3] & 0xff);
        m_fHighestAllowChargeCurrent = float(tempcur / 10.0);
        m_fCellCutoffVoltage = (float)(((data[4] & 0xff) << 8) | (data[5] & 0xff)) / 100.0;
        m_dMotiveBatteryStartSoc = data[6] & 0xff;
        m_dHighestAllowTemperature = data[7] & 0xff;
        break;
    case 0x0843:
        mRecv |= 0x04;
        m_fBmsReqVoltage = (float)(((data[0] & 0xff) << 8) | (data[1] & 0xff)) / 10.0;
        m_fBmsReqCurrent = (float)(((data[2] & 0xff) << 8) | (data[3] & 0xff)) / 10.0;
        m_dCellHighestVoltageGroup = (data[4] & 0xf0) >> 4;
        m_fCellHighestVoltage = (float)(((data[4] & 0x0f) << 8) | (data[5] & 0xff)) / 100.0;
        m_dBatteryHighestTemperature = (data[6] & 0xff) - 50;
        m_dBatteryLowestTemperature = (data[7] & 0xff) - 50;
        break;
    case 0x0844:
        mRecv |= 0x08;
        temp = data[0] & 0xff;
        if(temp == 1)
        {
            chargeType = CONSTANT_VOLTAGE;
        }
        else if (temp == 2) {
            chargeType = CONSTANT_CURRENT;
        }

        m_dCellHighestVoltageNO = data[1] & 0xff;
        m_dHighestTempNo = data[2] & 0xff;
        m_dLowestTempNo = data[3] & 0xff;
        for(int i = 0; i < m_bmsWarning.size(); i++)
        {
            m_bmsWarning.at(i)->setValue((m_bmsWarning.at(i)->getByte() >> m_bmsWarning.at(i)->getStartBit()) & 0x01);
        }
        break;
    default:
        break;
    }
}

void CConnectorStatus::handleHeartBeatMsg(unsigned char *data)
{
    // 心跳报文
    // 连接状态
    mRecv |= 0x400;
    int conState = data[2] & 0x01;
    lockState = (data[2] & 0x02) >> 1;
    int temp = (data[1] & 0xf0) >> 4;
    if((temp != chargerState) | (connectorState != conState))
    {
        // 状态变更
        // 发送状态变更消息到后台
        chargerState = temp;
        connectorState = conState;
        qDebug() << connectorCode << "charger state:" << chargerState << "    connector state:" << connectorState;

        CManage::getInstance()->sendChargerStateChangeToNetwork(packStateChangeMsg(), connectorCode);

        if((chargerState == 3) | (chargerState == 2))
        {
            CManage::getInstance()->sendMsgToMQ(CConnectorManager::getInstance()->getChargingMsgByCode(connectorCode), "statechange");

        }
        else
        {

            CManage::getInstance()->sendMsgToMQ(packChargeOrdinaryMsg(), "statechange");

            resetParam();
        }
    }

    // 解析告警数据
    for(int i = 0; i < m_connectorWarning.size(); i++)
    {
        m_connectorWarning.at(i)->setValue((m_connectorWarning.at(i)->getByte() >> m_connectorWarning.at(i)->getStartBit()) & 0x01);
    }
}

/*
 * 查询是否告警
*/
bool CConnectorStatus::isAlarm()
{
    bool bret = false;
    for(int i = 0; i < m_bmsWarning.size(); i++)
    {
        if(m_bmsWarning.at(i)->getValue() != 0)
        {
            bret = true;
        }
    }

    for(int i = 0; i < m_connectorWarning.size(); i++)
    {
        if(m_connectorWarning.at(i)->getValue() != 0)
        {
            bret = true;
        }
    }

    return true;
}

void CConnectorStatus::sendStatusToNetwork()
{
    CManage::getInstance()->sendChargerStateChangeToNetwork(packStateChangeMsg(), connectorCode);

    if((chargerState == 3) | (chargerState == 2))
    {
        CManage::getInstance()->sendMsgToMQ(CConnectorManager::getInstance()->getChargingMsgByCode(connectorCode), "statechange");
    }
    else
    {
        CManage::getInstance()->sendMsgToMQ(packChargeOrdinaryMsg(), "statechange");
    }
}

void CConnectorStatus::getSysErrCode(QSqlRecord rec)
{
    int code = rec.value(rec.indexOf("code")).toInt();
    if(code == 1)
    {
        CBaseWarning* warning = new CBaseWarning;
        warning->setConnectorCode(this->connectorCode);
        warning->setAlarmCode(rec.value(rec.indexOf("alarm_code")).toString());
        warning->setAlarmType(1);
        warning->setByte(rec.value(rec.indexOf("byte")).toInt());
        warning->setStartBit(rec.value(rec.indexOf("bit")).toInt());
        warning->setCode(1);
        warning->setLen(rec.value(rec.indexOf("length")).toInt());
        warning->setName(rec.value(rec.indexOf("name")).toString());
        warning->setName_cn(rec.value(rec.indexOf("name")).toString());
        warning->setName_en(rec.value(rec.indexOf("name_en")).toString());
        warning->setValue(0);

        m_bmsWarning.push_back(warning);
    }
    else if (code == 2) {
        CBaseWarning* warning = new CBaseWarning;

        warning->setConnectorCode(this->connectorCode);
        warning->setAlarmCode(rec.value(rec.indexOf("alarm_code")).toString());
        warning->setAlarmType(2);
        warning->setByte(rec.value(rec.indexOf("byte")).toInt());
        warning->setStartBit(rec.value(rec.indexOf("bit")).toInt());
        warning->setCode(2);
        warning->setLen(rec.value(rec.indexOf("length")).toInt());
        warning->setName(rec.value(rec.indexOf("name")).toString());
        warning->setName_cn(rec.value(rec.indexOf("name")).toString());
        warning->setName_en(rec.value(rec.indexOf("name_en")).toString());
        warning->setValue(0);

        m_connectorWarning.push_back(warning);
    }
}

/*
 * 打包状态变更报文
*/
QByteArray CConnectorStatus::packStateChangeMsg()
{
    QVariantMap param;
    param.insert("connectorCode", connectorCode);
    param.insert("stationCode", CApp::getInstance()->getSStationID());
    param.insert("connectorState", chargerState);
    if(connectorState == 1)
    {
        param.insert("connectedState", connectorState);
    }
    else
    {
        param.insert("connectedState", 2);
    }


    param.insert("orderDate", "");
    param.insert("currTime", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

    QByteArray data = QtJson::serialize(param);

    AESHandler handler;
    return handler.encodeMsg(data);
}

QByteArray CConnectorStatus::packChargeOrdinaryMsg()
{
    QVariantMap param;
    param.insert("connectorCode", connectorCode);
    param.insert("orderNo", "");
    param.insert("operatorCode", CApp::getInstance()->getSEnterpriseCode());
    param.insert("stationCode", CApp::getInstance()->getSStationID());
    param.insert("vin", "");
    param.insert("startSoc", 0);
    param.insert("soc", 0);
    param.insert("cardNo","");
    param.insert("startTime", "");
    param.insert("orderTime", "");
    param.insert("chargerCurrent", 0.0);
    param.insert("chargerVoltage", 0.0);
    param.insert("maxChargerModuleTemp", 0);
    param.insert("minChargerModuleTemp", 0);
    param.insert("chargerModuleCount", 8);
    param.insert("outputPower", 0);
    param.insert("bmsRequestVoltage", 0);
    param.insert("bmsRequestCurrent", 0);
    param.insert("batteryGroupId", "");
    param.insert("maxBatteryVoltage", 0);
    param.insert("money", 0);
    param.insert("elect", 0);
    param.insert("chargerDuration", 0);
    param.insert("batteryCapacity", 0);
    param.insert("batteryType", 0);
    param.insert("cellHighestVoltageGroupId",0);
    param.insert("cellHighestTemp", 0);
    param.insert("maxTempGroupId", 0);
    param.insert("cellLowestTemp", 0);
    param.insert("minTempGroupId", 0);
    param.insert("portTemp", QString::number(getDConnectorTemp()));
    param.insert("connectorState", QString::number(getChargerState()));
    if(getConnectorState() == 0)
    {
        param.insert("connectedState", QString::number(2));
    }
    else
    {
        param.insert("connectedState", QString::number(1));
    }

    param.insert("remainTime", 0);
    param.insert("chargingMode", getChargeType());
    param.insert("allowVoltage", 0);
    param.insert("allowCurrent", 0);
    param.insert("allowTemp", 0);

    QByteArray data = QtJson::serialize(param);

    AESHandler handler;

    return handler.encodeMsg(data);
}


void CConnectorStatus::setConnectorCode(QString code)
{
    this->connectorCode = code;
}

int CConnectorStatus::getChargerState() const
{
    return chargerState;
}

int CConnectorStatus::getConnectorState() const
{
    return connectorState;
}

int CConnectorStatus::getLockState() const
{
    return lockState;
}

double CConnectorStatus::getFVoltage() const
{
    return m_fVoltage;
}

double CConnectorStatus::getFCurrent() const
{
    return m_fCurrent;
}

int CConnectorStatus::getDStartSoc() const
{
    return m_dStartSoc;
}

int CConnectorStatus::getDEndSoc() const
{
    return m_dEndSoc;
}

int CConnectorStatus::getDCurrentSoc() const
{
    return m_dCurrentSoc;
}

int CConnectorStatus::getDConnectorTemp() const
{
    return m_dConnectorTemp;
}

int CConnectorStatus::getDRemainTime() const
{
    return m_dRemainTime;
}

double CConnectorStatus::getFAmount() const
{
    return m_fAmount;
}

int CConnectorStatus::getDChargingTime() const
{
    return m_dChargingTime;
}

double CConnectorStatus::getFQuantity() const
{
    return m_fQuantity;
}

BMSPROTOCOL CConnectorStatus::getBmsProtocol() const
{
    return bmsProtocol;
}

int CConnectorStatus::getDBatteryType() const
{
    return m_dBatteryType;
}

float CConnectorStatus::getFBatteryCapacity() const
{
    return m_fBatteryCapacity;
}

float CConnectorStatus::getFBatteryRatedVoltage() const
{
    return m_fBatteryRatedVoltage;
}

float CConnectorStatus::getFBatteryRatedPower() const
{
    return m_fBatteryRatedPower;
}

float CConnectorStatus::getFHighestAllowChargeVoltage() const
{
    return m_fHighestAllowChargeVoltage;
}

float CConnectorStatus::getFHighestAllowChargeCurrent() const
{
    return m_fHighestAllowChargeCurrent;
}

float CConnectorStatus::getFCellCutoffVoltage() const
{
    return m_fCellCutoffVoltage;
}

int CConnectorStatus::getDMotiveBatteryStartSoc() const
{
    return m_dMotiveBatteryStartSoc;
}

int CConnectorStatus::getDHighestAllowTemperature() const
{
    return m_dHighestAllowTemperature;
}

float CConnectorStatus::getFBmsReqVoltage() const
{
    return m_fBmsReqVoltage;
}

float CConnectorStatus::getFBmsReqCurrent() const
{
    return m_fBmsReqCurrent;
}

float CConnectorStatus::getFCellHighestVoltage() const
{
    return m_fCellHighestVoltage;
}

int CConnectorStatus::getDCellHighestVoltageGroup() const
{
    return m_dCellHighestVoltageGroup;
}

int CConnectorStatus::getDBatteryHighestTemperature() const
{
    return m_dBatteryHighestTemperature;
}

int CConnectorStatus::getDBatteryLowestTemperature() const
{
    return m_dBatteryLowestTemperature;
}

CHARGETYPE CConnectorStatus::getChargeType() const
{
    return chargeType;
}

int CConnectorStatus::getDCellHighestVoltageNO() const
{
    return m_dCellHighestVoltageNO;
}

int CConnectorStatus::getDHighestTempNo() const
{
    return m_dHighestTempNo;
}

int CConnectorStatus::getDLowestTempNo() const
{
    return m_dLowestTempNo;
}

float CConnectorStatus::getFACVoltageA() const
{
    return m_fACVoltageA;
}

float CConnectorStatus::getFACVoltageB() const
{
    return m_fACVoltageB;
}

float CConnectorStatus::getFACVoltageC() const
{
    return m_fACVoltageC;
}

float CConnectorStatus::getFACCurrentA() const
{
    return m_fACCurrentA;
}

float CConnectorStatus::getFACCurrentB() const
{
    return m_fACCurrentB;
}

float CConnectorStatus::getFACCurrentC() const
{
    return m_fACCurrentC;
}

QList<CBaseWarning *> CConnectorStatus::getBmsWarning() const
{
    return m_bmsWarning;
}

QList<CBaseWarning *> CConnectorStatus::getConnectorWarning() const
{
    return m_connectorWarning;
}

float CConnectorStatus::getFSharp() const
{
    return m_fSharp;
}

float CConnectorStatus::getFPeak() const
{
    return m_fPeak;
}

float CConnectorStatus::getFFlat() const
{
    return m_fFlat;
}

float CConnectorStatus::getFValley() const
{
    return m_fValley;
}

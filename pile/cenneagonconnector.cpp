#include "cenneagonconnector.h"
#include "manage/capp.h"
#include "manage/cmanage.h"

CEnneagonConnector::CEnneagonConnector(QObject *parent) : QObject(parent)
{
    m_bSyncTime = false;
    m_bConnectedToPile = false;
    m_dCounts = 0;
    m_bBookOverTime = false;
    m_bStartBook = false;
}

void CEnneagonConnector::showDetail()
{
    qDebug() << m_sChargerSn << " "<< m_sConnectorID << " " << QString::number(m_dDevID,16);
}

void CEnneagonConnector::init()
{
    m_dState = STANBY;
}

void CEnneagonConnector::readyToFinishOrder()
{
    // 20s time out
    // set order msg
    mOrder.setStartSoc(mStatus.getDStartSoc());
    mOrder.setEndSoc(mStatus.getDCurrentSoc());
    mOrder.setAmount(mStatus.getFAmount());
    mOrder.setQuantity(mStatus.getFQuantity());
    mOrder.setDuration(mStatus.getDChargingTime());
    mOrder.setEndmeter(mOrder.getStartmeter() + mOrder.getQuantity());
    mOrder.setSharp(mStatus.getFSharp());
    mOrder.setPeak(mStatus.getFPeak());
    mOrder.setFlat(mStatus.getFFlat());
    mOrder.setValley(mStatus.getFValley());
    mOrder.setEndTime(QDateTime::currentDateTime());

    QTimer::singleShot(10000, this, SLOT(completeOrder()));
}

void CEnneagonConnector::completeOrder()
{
//     complete order;
//     insert into database
//     send order to platform
    if(!mOrder.getBConsume())
    {
        mOrder.consumeOrder();
    }

    // reset all the param;
    mOrder.resetMainParam();
//    mOrder.resetParam();
//    mStatus.resetParam();
    mVerify.reset();
}

QString CEnneagonConnector::sChargerSn() const
{
    return m_sChargerSn;
}

/*
 *
*/
void CEnneagonConnector::updateCounts()
{
    m_dCounts += 1;
    if(m_dCounts > 30)
    {
        // 30s timeout
        // means the charger is offline
        m_bConnectedToPile = false;
        mStatus.setOfflineState();
    }
}

void CEnneagonConnector::setSChargerSn(const QString &sChargerSn)
{
    m_sChargerSn = sChargerSn;
}

QString CEnneagonConnector::sConnectorID() const
{
    return m_sConnectorID;
}

void CEnneagonConnector::setSConnectorID(const QString &sConnectorID)
{
    m_sConnectorID = sConnectorID;
    mVerify.setConnectorCode(sConnectorID);
    mStatus.setConnectorCode(sConnectorID);
    mOrder.setConnectorCode(sConnectorID);
    mRecordOrder.setConnectorCode(sConnectorID);
}

void CEnneagonConnector::updateStatus()
{
    mStatus.sendStatusToNetwork();
}

int CEnneagonConnector::dGun() const
{
    return m_dGun;
}

void CEnneagonConnector::setDGun(int dGun)
{
    m_dGun = dGun;
}

ushort CEnneagonConnector::dDevID() const
{
    return m_dDevID;
}

void CEnneagonConnector::setDDevID(const ushort &dDevID)
{
    m_dDevID = dDevID;
}

QString CEnneagonConnector::sSoftwareVersion() const
{
    return m_sSoftwareVersion;
}

void CEnneagonConnector::setSSoftwareVersion(const QString &sSoftwareVersion)
{
    m_sSoftwareVersion = sSoftwareVersion;
}

QString CEnneagonConnector::sHardwareVersion() const
{
    return m_sHardwareVersion;
}

void CEnneagonConnector::setSHardwareVersion(const QString &sHardwareVersion)
{
    m_sHardwareVersion = sHardwareVersion;
}

bool CEnneagonConnector::bIsReg() const
{
    return m_bIsReg;
}

void CEnneagonConnector::setBIsReg(bool bIsReg)
{
    m_bIsReg = bIsReg;
}

void CEnneagonConnector::decode(int msgType, unsigned char *data)
{
    /*
     *  根据帧id，对数据进行分类与组合
    */
    switch (msgType) {
    case 0x0884:
        mVerify.decodeCanMsg(msgType, data);
        mOrder.decodeCanMsg(msgType, data);
        break;
    case 0x0801:
    case 0x0802:
    case 0x1884:
        // 验证报文
        mVerify.decodeCanMsg(msgType, data);
        break;
    case 0x0880:
        m_bSyncTime = true;
        break;
    case 0x0840:
    case 0x0841:
    case 0x081b:
    case 0x081c:
    case 0x0812:
    case 0x0813:
    case 0x0826:
    case 0x0817:
    case 0x0818:
    case 0x081f:
    case 0x0820:
    case 0x0843:
    case 0x0844:
    case 0x0845:
    case 0x0846:
        mStatus.decodeCanMsg(msgType, data);
        break;
    case 0x0850:
    case 0x0842:
    case 0x0823:
    case 0x0816:
    case 0x0824:
    case 0x081d:
    case 0x081e:
    case 0x0814:
        mOrder.decodeCanMsg(msgType, data);
        break;
    case 0x1001:
    case 0x1003:
    case 0x1004:
    case 0x1005:
    case 0x1006:
    case 0x1007:
    case 0x1008:
    case 0x100a:
    case 0x100b:
    case 0x100c:
    case 0x1801:
    case 0x1802:
    case 0x1803:
        mRecordOrder.decodeCanMsg(msgType, data);
        break;
    case 0x0885:
        mOrder.decodeCanMsg(msgType, data);
        mStatus.decodeCanMsg(msgType, data);
        break;
    default:
        break;
    }

    m_dCounts = 0;
    m_bConnectedToPile = true;
}

bool CEnneagonConnector::bSyncTime() const
{
    return m_bSyncTime;
}

bool CEnneagonConnector::bConnectedToPile() const
{
    return m_bConnectedToPile;
}

void CEnneagonConnector::setRemoteOrderDetail(QString orderNo, QString phone)
{
    mOrder.setFlowNo(orderNo);
    mOrder.setPhoneNo(phone);
    mVerify.setFlowNo(orderNo);
}

void CEnneagonConnector::takeControl(int userChargeType, float vol, float cur, int mode, int value, QString account, int order)
{
    mOrder.setUserChargeType(QString::number(userChargeType));
    mOrder.setPaymentAccount(account);
    mOrder.setPayment(value);
    mOrder.setStartType(1);
    mVerify.setUserChargeType(QString::number(userChargeType));

    if(m_bStartBook == true){
        //已经开始预约。那么验证两个account 是否对应上，对应上就充，如果对应不上就不充.
        if(account == mVerify.getPersonalUserId()){
            m_bStartBook = false;
            mVerify.cancleBookOrder();

            // send command to charger
            CCANDataPakcer pakcer;
            pakcer.setDevID(this->dDevID());
            CManage::getInstance()->sendMsgToPile(pakcer.packControlMsg(order, mode, value));
        }
    }
    else {
        // send command to charger
        CCANDataPakcer pakcer;
        pakcer.setDevID(this->dDevID());
        CManage::getInstance()->sendMsgToPile(pakcer.packControlMsg(order, mode, value));
    }
}

void CEnneagonConnector::bookCharger(int userChargeType, QString bookdate, float vol, float cur, int mode, int value, QString account, int order, QString plate,QString overTime)
{
    CCANDataPakcer packer;
    packer.setDevID(dDevID());

    if(order == 1)
    {
        mOrder.setUserChargeType(QString::number(userChargeType));
        mOrder.setPaymentAccount(account);
        mOrder.setStartType(2);

        mVerify.setOrderTime(bookdate);
        mVerify.setOrderStartType(2);
        mVerify.setUserChargeType(QString::number(userChargeType));
        mVerify.setPersonalUserId(account);
        mVerify.setPlate(plate);
        mVerify.setOverTime(overTime);

        qDebug() << "userChargeType:" << userChargeType;

//        180512

//        if((userChargeType != 3) && (userChargeType != 4)){
        if((userChargeType != 3) && (userChargeType != 4) && (userChargeType != 7) && (userChargeType != 8) && (userChargeType != 9)){
            qDebug() << "send booking command:";
            // send booking command
            CManage::getInstance()->sendMsgToPile(packer.packBookingMsg(order, mode, value));
            // send booking date
            CManage::getInstance()->sendMsgToPile(packer.packBookingDateMsg(bookdate));
            // send booking setting
            CManage::getInstance()->sendMsgToPile(packer.packBookSettingMsg(100, 1));
            // send book account
            CManage::getInstance()->sendMsgToPile(packer.packBookingAccount(0, account.toLatin1()));
            CManage::getInstance()->sendMsgToPile(packer.packBookingAccount(1, account.toLatin1()));
            CManage::getInstance()->sendMsgToPile(packer.packBookingAccount(2, account.toLatin1()));
            CManage::getInstance()->sendMsgToPile(packer.packBookingAccount(3, account.toLatin1()));
        }
        else {
            // 创建预约订单.
            qDebug() << "send booking command:";
            QDateTime datetime = QDateTime::currentDateTime().addSecs(overTime.toInt() * 60);
            // send booking command
            CManage::getInstance()->sendMsgToPile(packer.packBookingMsg(order, mode, value));
            // send booking date
            CManage::getInstance()->sendMsgToPile(packer.packBookingDateMsg(datetime.toString("yyyy-MM-dd hh:mm:ss")));
            // send booking setting
            CManage::getInstance()->sendMsgToPile(packer.packBookSettingMsg(100, 1));
            // send book account
            CManage::getInstance()->sendMsgToPile(packer.packBookingAccount(0, account.toLatin1()));
            CManage::getInstance()->sendMsgToPile(packer.packBookingAccount(1, account.toLatin1()));
            CManage::getInstance()->sendMsgToPile(packer.packBookingAccount(2, account.toLatin1()));
            CManage::getInstance()->sendMsgToPile(packer.packBookingAccount(3, account.toLatin1()));

            if(mStatus.getChargerState() == 1){
                //mVerify.createBookOrder();

                m_bStartBook = true;
                m_bBookOverTime = false;

                qDebug() << "timeout :" << overTime.toInt() * 60;
                QTimer::singleShot(overTime.toInt() * 60 * 1000, this, SLOT(checkOverBookOrder()));
            }
        }
    }
    else if(order == 2)
    {
        mOrder.setUserChargeType("");
        mOrder.setPaymentAccount("");
        mOrder.setStartType(0);

        mVerify.setBCancleBook(1);
        m_bBookOverTime = false;
        m_bStartBook = false;
        // 取消预约
        CManage::getInstance()->sendMsgToPile(packer.packBookingMsg(order, mode, value));

//        180512
//        if((userChargeType != 3) && (userChargeType != 4)){
        if((userChargeType != 3) && (userChargeType != 4) && (userChargeType != 7) && (userChargeType != 8) && (userChargeType != 9)){

        }
        else {
            if(mStatus.getChargerState() == 1){
                //mVerify.cancleBookOrder();
                m_bStartBook = false;
                m_bBookOverTime = false;
            }
        }
    }
}

void CEnneagonConnector::checkOverBookOrder()
{
    qDebug() << "qtsingle shot timeout";
    if(m_bStartBook == true){
        // 已经开始预约，预约超时，创建取消预约订单
        mVerify.cancleBookOrder();
        m_bStartBook = false;
        m_bBookOverTime = false;
    }
}

void CEnneagonConnector::updatePlatfromConsumeRetMsg(QString flowNo, bool ret)
{

}

CVerify CEnneagonConnector::verify() const
{
    return mVerify;
}

CConnectorOrder CEnneagonConnector::order() const
{
    return mOrder;
}

CConnectorStatus CEnneagonConnector::status() const
{
    return mStatus;
}

void CEnneagonConnector::setStartType(const QString &userChargeType, const QString &account, const QString &channel)
{
    mOrder.setUserChargeType(userChargeType);
    mOrder.setPaymentAccount(account);
    mOrder.setPaymentChannel(channel);
}

void CEnneagonConnector::setCardNo(const QString &cardNO)
{
    mOrder.setCardNO(cardNO);
}

QByteArray CEnneagonConnector::packRegisteMsg()
{
    QVariantMap param;
    param.insert("equipmentCode", m_sChargerSn);
    param.insert("hardwareVersion", m_sHardwareVersion);
    param.insert("softwareVersion", m_sSoftwareVersion);
    param.insert("currTime", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

    QByteArray data = QtJson::serialize(param);
    AESHandler handler;
    return handler.encodeMsg(data);
}


QByteArray CEnneagonConnector::packChargingMsg()
{
    QVariantMap param;
    param.insert("connectorCode", m_sConnectorID);
    param.insert("orderNo", mOrder.getFlowNo());
    param.insert("operatorCode", CApp::getInstance()->getSEnterpriseCode());
    param.insert("stationCode", CApp::getInstance()->getSStationID());
    param.insert("userChargeType", mVerify.getUserChargeType());
    param.insert("vin", mOrder.getVinStr());
    param.insert("startSoc", mStatus.getDStartSoc());
    param.insert("soc", mStatus.getDCurrentSoc());
    param.insert("cardNo",mVerify.getCardNoStr());
    param.insert("startTime", mOrder.getStartTime().toString("yyyy-MM-dd hh:mm:ss"));
    param.insert("orderTime", mVerify.getOrderTime());
    param.insert("chargerCurrent", QString::number(mStatus.getFCurrent(), 'f', 2));
    param.insert("chargerVoltage", QString::number(mStatus.getFVoltage(), 'f', 2));
    param.insert("maxChargerModuleTemp", QString::number(mStatus.getDBatteryHighestTemperature()));
    param.insert("minChargerModuleTemp", QString::number(mStatus.getDBatteryLowestTemperature()));
    param.insert("chargerModuleCount", 8);
    param.insert("outputPower", QString::number(mStatus.getFCurrent() * mStatus.getFVoltage() / 1000, 'f', 2));
    param.insert("bmsRequestVoltage", QString::number(mStatus.getFBmsReqVoltage(), 'f', 2));
    param.insert("bmsRequestCurrent", QString::number(mStatus.getFBmsReqCurrent(), 'f', 2));
    param.insert("batteryGroupId", "");
    param.insert("maxBatteryVoltage", QString::number(mStatus.getFCellHighestVoltage(), 'f', 2));
    param.insert("money", QString::number(mStatus.getFAmount(), 'f', 2));
    param.insert("elect", QString::number(mStatus.getFQuantity(), 'f', 2));
    param.insert("chargerDuration", QString::number(mStatus.getDChargingTime()));
    param.insert("batteryCapacity", QString::number(mStatus.getFBatteryRatedPower(), 'f', 2));
    param.insert("batteryType", QString::number(mStatus.getDBatteryType()));
    param.insert("cellHighestVoltageGroupId", QString::number(mStatus.getDCellHighestVoltageGroup()));
    param.insert("cellHighestTemp", QString::number(mStatus.getDBatteryHighestTemperature()));
    param.insert("maxTempGroupId", QString::number(mStatus.getDHighestTempNo()));
    param.insert("cellLowestTemp", QString::number(mStatus.getDBatteryLowestTemperature()));
    param.insert("minTempGroupId", QString::number(mStatus.getDLowestTempNo()));
    param.insert("portTemp", QString::number(mStatus.getDConnectorTemp()));
    param.insert("connectorState", QString::number(mStatus.getChargerState()));
    if(mStatus.getConnectorState() == 0)
    {
        param.insert("connectedState", QString::number(2));
    }
    else
    {
        param.insert("connectedState", QString::number(mStatus.getConnectorState()));
    }

    param.insert("remainTime", QString::number(mStatus.getDRemainTime()));
    param.insert("chargingMode", mStatus.getChargeType());
    param.insert("allowVoltage", QString::number(mStatus.getFHighestAllowChargeVoltage(), 'f', 2));
    param.insert("allowCurrent", QString::number(mStatus.getFHighestAllowChargeCurrent(), 'f', 2));
    param.insert("allowTemp", QString::number(mStatus.getDHighestAllowTemperature()));

    QByteArray data = QtJson::serialize(param);

    AESHandler handler;

    return handler.encodeMsg(data);
}

#include "cconnectororder.h"
#include "manage/capp.h"
#include "manage/cmanage.h"

CConnectorOrder::CConnectorOrder()
{
    resetParam();
}

void CConnectorOrder::resetMainParam()
{
    mRecv = 0;
    // 默认直接启动
    startType = 1;
    bStartCharging = false;
    m_consumeRecv = 0;
    //bConsume = false;
    bCreateSuccess = false;
    flowNo = "";
    phoneNo = "";
    connectorFlowNo = "";
    userChargeType = "";
    vinStr = "";
    startSoc = 0;
    endSoc = 0;
    duration = 0;
    startmeter = 0;
    endmeter = 0;
    stopType = 0;
    payment = 0;
    paymentAccount = "";
    paymentChannel = "";
    cardNO = "";
    amount = 0.0;
    elecmoney = 0.0;
    servicemoney = 0.0;
    quantity = 0.0;
    sharp = 0.0;
    peak = 0.0;
    flat = 0.0;
    valley = 0.0;
}

void CConnectorOrder::resetParam()
{
    mRecv = 0;
    // 默认直接启动
    startType = 1;
    bStartCharging = false;
    m_consumeRecv = 0;
    //bConsume = false;
    bCreateSuccess = false;
    flowNo = "";
    phoneNo = "";
    connectorFlowNo = "";
    userChargeType = "";
    vinStr = "";
    startSoc = 0;
    endSoc = 0;
    duration = 0;
    startmeter = 0;
    endmeter = 0;
    stopType = 0;
    payment = 0;
    paymentAccount = "";
    paymentChannel = "";
    cardNO = "";
    amount = 0.0;
    elecmoney = 0.0;
    servicemoney = 0.0;
    quantity = 0.0;
    sharp = 0.0;
    peak = 0.0;
    flat = 0.0;
    valley = 0.0;
}

// 根据充电桩的反馈来进行订单的创建更新以及重置
void CConnectorOrder::decodeCanMsg(int msgType, unsigned char *data)
{
    int temp = 0;
    int tempa = 0;
    int command = 0;
    int res = 0;
    QString dateStr = "";

    CCANDataPakcer packer;
    packer.setDevID(CConnectorManager::getInstance()->getDevIDByConnetorID(connectorCode));
    switch (msgType) {
    case 0x1801:
        CManage::getInstance()->sendMsgToPile(packer.packSuccessReturnMsg(0x1881));
        break;
    case 0x1802:
        // 是否预约成功
        temp = data[0] & 0xff;
        if(temp == 1)
        {
            startType = 2;
        }
        break;
    case 0x1803:
        // 预约时间, 没办法往上送
        CManage::getInstance()->sendMsgToPile(packer.packBookSuccessMsg(0x1883, 1, 1));
        break;
    case 0x0885:
        // 根据心跳报文来判定，是否需要对账单进行结算
        // 枪状态
        temp = (data[1] & 0xf0) >> 4;

        //180511

//        if((userChargeType.compare("3") == 0) && (userChargeType.compare("4") == 0)){

//        }else{

        if(isOrderNeedToConsume(temp)){

            CConnectorManager::getInstance()->setReadyToCompleteOrder(connectorCode);

            CManage::getInstance()->sendMsgToPile(packer.packControlMsg(2,0,0));
        }

//        }


        //180511

        break;
    case 0x0850:
        temp = data[0] & 0xff;
        if(temp == 1)
        {
            // 正式开始充电
            // 创建流水号
            if(bStartCharging == false)
            {
                mRecv = 0;
                m_consumeRecv = 0;
                mRecv |= 0x01;
                createFlowNo();
                startTime = QDateTime::currentDateTime();
                // 返回流水号到充电桩
                bStartCharging = true;
                bConsume = false;

                CManage::getInstance()->sendMsgToPile(packer.packStartTransactionMsg(1, 1, 0, connectorFlowNo));
            }

            CManage::getInstance()->sendMsgToPile(packer.packStartTransactionMsg(1, 1, 0, connectorFlowNo));
        }
        else if(temp == 2)
        {
            // 充电结束，更新订单的状态。
            //
            //bStartCharging = false;
            mRecv = 0;
            m_consumeRecv = 0;
            CCANDataPakcer packer;
            packer.setDevID(CConnectorManager::getInstance()->getDevIDByConnetorID(connectorCode));

            CManage::getInstance()->sendMsgToPile(packer.packStopTransactionMsg(2, 2, 0, 0, 0));

            CManage::getInstance()->sendMsgToPile(packer.packQueryConsumeMsg(connectorFlowNo));
            endTime = QDateTime::currentDateTime();

            //
            CConnectorManager::getInstance()->setReadyToCompleteOrder(connectorCode);
        }
        break;
    case 0x0842:
        // vin
        if((data[0] & 0xff) == 0x01)
        {
            mRecv |= 0x02;
            vin[0] = data[1];
            vin[1] = data[2];
            vin[2] = data[3];
            vin[3] = data[4];
            vin[4] = data[5];
            vin[5] = data[6];
            vin[6] = data[7];
        }
        else if((data[0] & 0xff) == 0x02)
        {
            mRecv |= 0x04;
            vin[7] = data[1];
            vin[8] = data[2];
            vin[9] = data[3];
            vin[10] = data[4];
            vin[11] = data[5];
            vin[12] = data[6];
            vin[13] = data[7];
        }
        else if((data[0] & 0xff) == 0x03)
        {
            mRecv |= 0x08;
            vin[14] = data[1];
            vin[15] = data[2];
            vin[16] = data[3];
        }
        break;
    case 0x0816:
        // start time and end time
        if((data[7] & 0xff) == 0x00)
        {
            m_consumeRecv |= 0x01;
            startSoc = data[6] & 0xff;
        }
        else
        {
            m_consumeRecv |= 0x02;
//            dateStr = QString("%1").arg(2000 + data[0] & 0xff, 4, 10, QChar('0')) + "-" +
//                    QString("%1").arg(data[1] & 0xff, 2, 10, QChar('0')) + "-" +
//                    QString("%1").arg(data[2] & 0xff, 2, 10, QChar('0')) + " " +
//                    QString("%1").arg(data[3] & 0xff, 2, 10, QChar('0')) + ":" +
//                    QString("%1").arg(data[4] ^ 0xff, 2, 10, QChar('0')) + ":" +
//                    QString("%1").arg(data[5] & 0xff, 2, 10, QChar('0'));
//            endTime = QDateTime::fromString(dateStr, "yyyy-MM-dd hh:mm:ss");
            endSoc = data[6] & 0xff;
        }
        break;
    case 0x0823:
        m_consumeRecv |= 0x04;
        amount = (float)((((data[0] & 0xff) << 8) | (data[1] & 0xff))) / 100.0;
        duration = (((data[2] & 0xff) << 8) | (data[3] & 0xff));
        quantity = (float)(((data[4] & 0xff) << 24) | ((data[5] & 0xff) << 16) | ((data[6] & 0xff) << 8) | (data[7] & 0xff)) / 100.0;
        break;
    case 0x081d:
        m_consumeRecv |= 0x08;
        sharp = (float)(((data[0] & 0xff) << 24) | ((data[1] & 0xff) << 16) | ((data[2] & 0xff) << 8) | (data[3] & 0xff)) / 100.0;
        peak = (float)(((data[4] & 0xff) << 24) | ((data[5] & 0xff) << 16) | ((data[6] & 0xff) << 8) | (data[7] & 0xff)) / 100.0;
        break;
    case 0x0814:
        if((data[4] & 0xff) == 0x00)
        {
            m_consumeRecv |= 0x10;
            startmeter = (float)(((data[0] & 0xff) << 24) | ((data[1] & 0xff) << 16) | ((data[2] & 0xff) << 8) | (data[3] & 0xff)) / 100.0;
            CManage::getInstance()->sendMsgToPile(packer.packReturnMsg(0x0814, true));
        }
        else
        {
            m_consumeRecv |= 0x20;
            endmeter = (float)(((data[0] & 0xff) << 24) | ((data[1] & 0xff) << 16) | ((data[2] & 0xff) << 8) | (data[3] & 0xff)) / 100.0;
            CManage::getInstance()->sendMsgToPile(packer.packReturnMsg(0x0814, true));
        }
        break;
    case 0x081e:
        m_consumeRecv |= 0x40;
        flat = (float)(((data[0] & 0xff) << 24) | ((data[1] & 0xff) << 16) | ((data[2] & 0xff) << 8) | (data[3] & 0xff)) / 100.0;
        valley = (float)(((data[4] & 0xff) << 24) | ((data[5] & 0xff) << 16) | ((data[6] & 0xff) << 8) | (data[7] & 0xff)) / 100.0;
        break;
    case 0x0824:
        m_consumeRecv |= 0x80;
        temp = data[0] & 0x03;
        tempa = (data[0] >> 2) & 0x03;
        if(tempa == 0 )
        {
            stopType = 1;
        }
        else if (tempa == 2) {
            stopType = 2;
        }
        else if(tempa == 1)
        {
            if(temp == 0)
            {
                stopType = 3;
            }
            else {
                stopType = 4;
            }
        }
        break;
    case 0x0884:
        // 启动命令返回结果，如果启动失败，并且已经生成了充电订单，那么将调用退款接口
        // 根据用户类型，产生退款订单号，调用退款即可
        command = data[0] & 0x0f;
        res = (data[0] & 0x10) >> 4;
        if((command == 1) && (res == 0))
        {
            if(userChargeType.compare("7") == 0)
            {
                CManage::getInstance()->sendWechatRefund(packWechatRefund());
            }
            else if(userChargeType.compare("9") == 0)
            {
                CManage::getInstance()->sendAliRefund(packAliRefund());
            }
            else if(userChargeType.compare("11") == 0){
                stopType = 5;
            }
        }
        break;
    default:
        break;
    }

    if(((m_consumeRecv & 0xff) == 0xff) && (bStartCharging == true) && (bConsume == false))
    {
        /*
         * consume
        */
        m_consumeRecv = 0;
        bConsume = true;
        bStartCharging = false;
        consumeOrder();
    }

    if((mRecv & 0x0f) == 0x0f)
    {
        mRecv = 0;
        handleVin();
        if(bCreateSuccess == false)
        {
            // 发送订单创建报文
            CManage::getInstance()->sendFlowNoRequest(connectorCode, flowNo, packOrderCreateMsg());

            CManage::getInstance()->sendMsgToPile(packer.packReturnMsg(0x0842, true));
        }
    }
}

bool CConnectorOrder::isOrderNeedToConsume(int state)
{
    QDateTime cur = QDateTime::currentDateTime();
    int sec = startTime.msecsTo(cur) / 1000;
    if((state == 3) && (bStartCharging == true) && (bConsume == false) && (sec > 120))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void CConnectorOrder::consumeOrder()
{
    bConsume = true;
    saveOrder();
    // send order msg to platform
    CManage::getInstance()->sendConsumeMsgToNetwork(connectorCode, flowNo, packOrderMsg());
}

void CConnectorOrder::handleVin()
{
    bool bAll = true;
    vinStr = "";

    for(int i = 0; i < 17; i++)
    {
        QChar te(vin[i]);
        if(te.isLetterOrNumber() && (vin[i] != 0xff))
        {
            vinStr += vin[i];
        }
        else
        {
            bAll = false;
        }
    }

    if(bAll == false)
    {
        vinStr = "";
    }
}

QByteArray CConnectorOrder::packOrderCreateMsg()
{
    QVariantMap param;
    param.insert("orderNo", flowNo);
    param.insert("stationCode", CApp::getInstance()->getSStationID());
    param.insert("equipmentCode", connectorCode.left(connectorCode.length() - 1));
    param.insert("connectorCode", connectorCode);
    param.insert("paymentAccount", paymentAccount);
    param.insert("vin", vinStr);
    param.insert("userChargeType", userChargeType);
    param.insert("startTime", startTime.toString("yyyy-MM-dd hh:mm:ss"));
    param.insert("startType", startType);
    param.insert("currTime", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    param.insert("mobileNumber", phoneNo);
    QByteArray data = QtJson::serialize(param);

    AESHandler handler;
    return handler.encodeMsg(data);
}

QByteArray CConnectorOrder::packAliRefund()
{
    QVariantMap param;
    param.insert("outTradeNo", paymentAccount);
    param.insert("refundFee",QString::number(payment));
    param.insert("currTime", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    QByteArray data = QtJson::serialize(param);

    qDebug() << "ali refund:" << data;
    AESHandler handler;
    return handler.encodeMsg(data);
}

QByteArray CConnectorOrder::packWechatRefund()
{
    QVariantMap param;
    param.insert("outTradeNo", paymentAccount);
    param.insert("refundFee", QString::number(payment));
    param.insert("currTime", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:dd"));
    QByteArray data = QtJson::serialize(param);
    qDebug() << "wechat refund:" << data;
    AESHandler handler;
    return handler.encodeMsg(data);
}

void CConnectorOrder::saveOrder()
{
    QStringList param;
    QStringList value;
    param << "flowno" << "connectorflowno" << "connectorcode" << "startsoc"
                      << "endsoc" << "amount" << "quantity" << "starttime" << "endtime"
                      << "chargetime" << "startmeter" << "endmeter" << "sharp" << "peak"
                      << "flat" << "valley" << "cardno" << "vin" << "stoptype" << "finished" << "send_success"
                      << "userChargeType" << "paymentAccount" << "starttype";
    value << flowNo << connectorFlowNo << connectorCode << QString::number(startSoc)
          << QString::number(endSoc) << QString::number(amount, 'f', 2) << QString::number(quantity, 'f', 2) << startTime.toString("yyyy-MM-dd hh:mm:ss") << endTime.toString("yyyy-MM-dd hh:mm:ss")
          << QString::number(duration) << QString::number(startmeter, 'f', 2) << QString::number(endmeter, 'f', 2) << QString::number(sharp, 'f', 2) << QString::number(peak, 'f', 2)
          << QString::number(flat, 'f', 2) << QString::number(valley, 'f', 2) << cardNO << vinStr << QString::number(stopType) << QString::number(1) << QString::number(0)
          << userChargeType << paymentAccount << QString::number(startType);

    QString table = "consume";
    CPileDataHelper::getInstance()->insert(table, param, value);
}

QByteArray CConnectorOrder::packOrderMsg()
{
    QVariantMap param;
    param.insert("orderNo", flowNo);
    param.insert("operatorCode", CApp::getInstance()->getSEnterpriseCode());
    param.insert("userChargeType", userChargeType);
    param.insert("endTime", endTime.toString("yyyy-MM-dd hh:mm:ss"));
    param.insert("startSoc", startSoc);
    param.insert("endSoc", endSoc);
    param.insert("chargerDuration", duration);
    param.insert("stopType", stopType);
    param.insert("paymentAmount",  QString::number(amount, 'f', 2));
    param.insert("paymentAccount", paymentAccount);
    param.insert("payChannel", paymentChannel);
    param.insert("physicsCardNo", cardNO);
    param.insert("money", QString::number(amount, 'f', 2));
    param.insert("electMoney", QString::number(amount, 'f', 2));
    float servicemoney = CApp::getInstance()->getSharpService() * sharp + CApp::getInstance()->getPeakService() * peak + CApp::getInstance()->getFlatService() * flat + CApp::getInstance()->getValleyService() * valley;
    param.insert("serviceMoney", QString::number(servicemoney,'f', 2));
    param.insert("elect", QString::number(quantity, 'f', 2));
    param.insert("cuspElect", QString::number(sharp, 'f', 2));
    param.insert("cuspElectPrice", QString::number(CApp::getInstance()->getSharpRate(), 'f', 2));
    param.insert("cuspServicePrice", QString::number(CApp::getInstance()->getSharpService(), 'f', 2));
    param.insert("cuspMoney", QString::number(CApp::getInstance()->getSharpTotal() * sharp , 'f', 2));
    param.insert("cuspElectMoney", QString::number(CApp::getInstance()->getSharpRate() * sharp, 'f', 2));
    param.insert("cuspServiceMoney", QString::number(CApp::getInstance()->getSharpService() * sharp, 'f', 2));
    param.insert("peakElect", QString::number(peak, 'f', 2));
    param.insert("peakElectPrice", QString::number(CApp::getInstance()->getPeakRate(), 'f', 2));
    param.insert("peakServicePrice", QString::number(CApp::getInstance()->getPeakService(), 'f', 2));
    param.insert("peakMoney", QString::number(CApp::getInstance()->getpeakTotal() * peak, 'f', 2));
    param.insert("peakElectMoney", QString::number(peak * CApp::getInstance()->getPeakRate(), 'f', 2));
    param.insert("peakServiceMoney", QString::number(peak * CApp::getInstance()->getPeakService(), 'f', 2));
    param.insert("flatElect", QString::number(flat, 'f', 2));
    param.insert("flatElectPrice", QString::number(CApp::getInstance()->getFlatRate(), 'f', 2));
    param.insert("flatServicePrice", QString::number(CApp::getInstance()->getFlatService(), 'f', 2));
    param.insert("flatMoney", QString::number(flat * CApp::getInstance()->getFlatTotal(), 'f', 2));
    param.insert("flatElectMoney", QString::number(flat * CApp::getInstance()->getFlatRate(), 'f', 2));
    param.insert("flatServiceMoney", QString::number(flat * CApp::getInstance()->getFlatService(), 'f', 2));
    param.insert("valleyElect", QString::number( valley, 'f', 2));
    param.insert("valleyElectPrice", QString::number(CApp::getInstance()->getValleyRate(), 'f', 2));
    param.insert("valleyServicePrice", QString::number(CApp::getInstance()->getValleyService(), 'f', 2));
    param.insert("valleyMoney", QString::number(valley * CApp::getInstance()->getValleyTotal(), 'f', 2));
    param.insert("valleyElectMoney", QString::number(valley * CApp::getInstance()->getValleyRate(), 'f', 2));
    param.insert("valleyServiceMoney", QString::number( valley * CApp::getInstance()->getValleyService(), 'f', 2));
    param.insert("currTime", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    QByteArray data = QtJson::serialize(param);

    qDebug() << "consume msg" << data;

    AESHandler handler;
    return handler.encodeMsg(data);
}

void CConnectorOrder::createFlowNo()
{
    QString no = "";
    no += CApp::getInstance()->getSEnterpriseCode();

    QDateTime cur = QDateTime::currentDateTime();

    no += cur.toString("yyMMddhhmmsszzz");

    no += connectorCode.right(3);

    //
    uint aa = 0;
    for(int i = 0; i < no.size(); i++)
    {
        aa += no.at(i).toLatin1();
    }

    int bb = aa % 10;

    no += QString("%1").arg(bb, 2, 10, QChar('0'));

    if(userChargeType.compare("11") != 0){
        flowNo = no;
    }

    connectorFlowNo = cur.toString("yyMMddhhmmss");
}

QString CConnectorOrder::getConnectorCode() const
{
    return connectorCode;
}

void CConnectorOrder::setConnectorCode(const QString &value)
{
    connectorCode = value;
}

QString CConnectorOrder::getFlowNo() const
{
    return flowNo;
}

void CConnectorOrder::setFlowNo(const QString &value)
{
    flowNo = value;
}

QString CConnectorOrder::getUserChargeType() const
{
    return userChargeType;
}

void CConnectorOrder::setUserChargeType(const QString &value)
{
    userChargeType = value;
}

QDateTime CConnectorOrder::getStartTime() const
{
    return startTime;
}

void CConnectorOrder::setStartTime(const QDateTime &value)
{
    startTime = value;
}

QDateTime CConnectorOrder::getEndTime() const
{
    return endTime;
}

void CConnectorOrder::setEndTime(const QDateTime &value)
{
    endTime = value;
}

int CConnectorOrder::getStartSoc() const
{
    return startSoc;
}

void CConnectorOrder::setStartSoc(int value)
{
    startSoc = value;
}

int CConnectorOrder::getEndSoc() const
{
    return endSoc;
}

void CConnectorOrder::setEndSoc(int value)
{
    endSoc = value;
}

int CConnectorOrder::getDuration() const
{
    return duration;
}

void CConnectorOrder::setDuration(int value)
{
    duration = value;
}

int CConnectorOrder::getStopType() const
{
    return stopType;
}

void CConnectorOrder::setStopType(int value)
{
    stopType = value;
}

int CConnectorOrder::getPayment() const
{
    return payment;
}

void CConnectorOrder::setPayment(int value)
{
    payment = value;
}

QString CConnectorOrder::getPaymentAccount() const
{
    return paymentAccount;
}

void CConnectorOrder::setPaymentAccount(const QString &value)
{
    paymentAccount = value;
}

QString CConnectorOrder::getPaymentChannel() const
{
    return paymentChannel;
}

void CConnectorOrder::setPaymentChannel(const QString &value)
{
    paymentChannel = value;
}

QString CConnectorOrder::getCardNO() const
{
    return cardNO;
}

void CConnectorOrder::setCardNO(const QString &value)
{
    cardNO = value;
}

float CConnectorOrder::getAmount() const
{
    return amount;
}

void CConnectorOrder::setAmount(float value)
{
    amount = value;
}

float CConnectorOrder::getElecmoney() const
{
    return elecmoney;
}

void CConnectorOrder::setElecmoney(float value)
{
    elecmoney = value;
}

float CConnectorOrder::getQuantity() const
{
    return quantity;
}

void CConnectorOrder::setQuantity(float value)
{
    quantity = value;
}

float CConnectorOrder::getSharp() const
{
    return sharp;
}

void CConnectorOrder::setSharp(float value)
{
    sharp = value;
}

float CConnectorOrder::getPeak() const
{
    return peak;
}

void CConnectorOrder::setPeak(float value)
{
    peak = value;
}

float CConnectorOrder::getFlat() const
{
    return flat;
}

void CConnectorOrder::setFlat(float value)
{
    flat = value;
}

float CConnectorOrder::getValley() const
{
    return valley;
}

void CConnectorOrder::setValley(const float &value)
{
    valley = value;
}

QString CConnectorOrder::getVinStr() const
{
    return vinStr;
}

void CConnectorOrder::setVinStr(const QString &value)
{
    vinStr = value;
}

QString CConnectorOrder::getConnectorFlowNo() const
{
    return connectorFlowNo;
}

void CConnectorOrder::setConnectorFlowNo(const QString &value)
{
    connectorFlowNo = value;
}

int CConnectorOrder::getStartType() const
{
    return startType;
}

void CConnectorOrder::setStartType(int value)
{
    startType = value;
}

bool CConnectorOrder::getBCreateSuccess() const
{
    return bCreateSuccess;
}

void CConnectorOrder::setBCreateSuccess(bool value)
{
    bCreateSuccess = value;
}

bool CConnectorOrder::getBStartCharging() const
{
    return bStartCharging;
}

void CConnectorOrder::setBStartCharging(bool value)
{
    bStartCharging = value;
}

bool CConnectorOrder::getBConsume() const
{
    return bConsume;
}

void CConnectorOrder::setBConsume(bool value)
{
    bConsume = value;

    CCANDataPakcer packer;
    packer.setDevID(CConnectorManager::getInstance()->getDevIDByConnetorID(connectorCode));
    CManage::getInstance()->sendMsgToPile(packer.packStopTransactionMsg(2, 3, 0, 0, 0));

    usleep(100 * 1000);

    CManage::getInstance()->sendMsgToPile(packer.packReturnMsg(0x081d, true));

    /*
     * update database status
    */
    if(value)
    {
        CPileDataHelper::getInstance()->completeOrder(flowNo);
    }

    resetParam();
}

float CConnectorOrder::getStartmeter() const
{
    return startmeter;
}

float CConnectorOrder::getEndmeter() const
{
    return endmeter;
}

void CConnectorOrder::setStartmeter(float value)
{
    startmeter = value;
}

void CConnectorOrder::setEndmeter(float value)
{
    endmeter = value;
}

QString CConnectorOrder::getPhoneNo() const
{
    return phoneNo;
}

void CConnectorOrder::setPhoneNo(const QString &value)
{
    phoneNo = value;
}

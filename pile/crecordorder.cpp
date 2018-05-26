#include "crecordorder.h"
#include "manage/capp.h"
#include "manage/cmanage.h"

CRecordOrder::CRecordOrder()
{
    mRecv = 0;
}

void CRecordOrder::resetParam()
{
    mRecv = 0;
}

// 根据充电桩的反馈来进行订单的创建更新以及重置
void CRecordOrder::decodeCanMsg(int msgType, unsigned char *data)
{
    int temp = 0;
    int tempa = 0;
    CCANDataPakcer packer;
    packer.setDevID(CConnectorManager::getInstance()->getDevIDByConnetorID(connectorCode));
    switch (msgType) {
    case 0x1001:
        // confirm record data
        CManage::getInstance()->sendMsgToPile(packer.packRecordQueryMsg(0x1002));

        mRecv = 0;
        break;
    case 0x1003:
        // card no
        mRecv |= 0x01;
        cardNO = "";
        for(int i = 0; i < 4; i++)
        {
            if(data[i] != 0x00)
            {
                //cardNO += QString::number(data[i], 16);
                cardNO += QString("%1").arg(data[i], 2, 16, QChar('0'));
            }
        }
        if(!cardNO.isEmpty())
        {
            startType = 1;
        }
        break;
    case 0x1004:
        if((data[7] & 0xff) == 0x00)
        {
            // 开始时间
            mRecv |= 0x02;
            startSoc = data[6];
            startTime.setDate(QDate(2000 + data[0], data[1], data[2]));
            startTime.setTime(QTime(data[3], data[4], data[5]));
        }
        else if((data[7] & 0xff) == 0x01)
        {
            // 结束时间
            mRecv |= 0x04;
            endSoc = data[6];
            endTime.setDate(QDate(2000 + data[0], data[1], data[2]));
            endTime.setTime(QTime(data[3], data[4], data[5]));
        }
        break;
    case 0x1005:
        // 消费信息
        mRecv |= 0x08;
        amount = (float)(((data[0] & 0xff) << 8) | ((data[1] & 0xff))) / 100.0;
        duration = ((data[2] & 0xff) << 8) | (data[3] & 0xff);
        quantity = (((data[4] & 0xff) << 24) | ((data[5] & 0xff) << 16) | ((data[6] & 0xff) << 8) | ((data[7] & 0xff))) / 100.0;
        break;
    case 0x1006:
        if((data[4] & 0xff) == 0x00)
        {
            //开始电度
            mRecv |= 0x10;
            temp = (((data[0] & 0xff) << 24) | ((data[1] & 0xff) << 16) | ((data[2] & 0xff) << 8) | (data[3] & 0xff));
            startmeter = (float)(temp) / 100.0;
        }
        else if((data[4] & 0xff) == 0x01)
        {
            mRecv |= 0x20;
            temp = (((data[0] & 0xff) << 24) | ((data[1] & 0xff) << 16) | ((data[2] & 0xff) << 8) | (data[3] & 0xff));
            endmeter = (float)(temp) / 100.0;
        }

        break;
    case 0x1007:
        // 尖峰
        mRecv |= 0x40;
        temp = (((data[0] & 0xff) << 24) | ((data[1] & 0xff) << 16) | ((data[2] & 0xff) << 8) | (data[3] & 0xff));
        sharp = (float)(temp) / 100.0;
        temp = (((data[4] & 0xff) << 24) | ((data[5] & 0xff) << 16) | ((data[6] & 0xff) << 8) | (data[7] & 0xff));
        peak = (float)(temp) / 100.0;
        break;
    case 0x1008:
        // 峰
        mRecv |= 0x80;
        temp = (((data[0] & 0xff) << 24) | ((data[1] & 0xff) << 16) | ((data[2] & 0xff) << 8) | (data[3] & 0xff));
        flat = (float)(temp) / 100.0;
        temp = (((data[4] & 0xff) << 24) | ((data[5] & 0xff) << 16) | ((data[6] & 0xff) << 8) | (data[7] & 0xff));
        valley = (float)(temp) / 100.0;
        break;
    case 0x100a:
        //历史记录车辆VIN
        if(data[0] == 0x01)
        {
            mRecv |= 0x100;
            vin[0] = data[1];
            vin[1] = data[2];
            vin[2] = data[3];
            vin[3] = data[4];
            vin[4] = data[5];
            vin[5] = data[6];
            vin[6] = data[7];
        }
        else if(data[0] == 0x02)
        {
            mRecv |= 0x200;
            vin[7] = data[1];
            vin[8] = data[2];
            vin[9] = data[3];
            vin[10] = data[4];
            vin[11] = data[5];
            vin[12] = data[6];
            vin[13] = data[7];
        }
        else if(data[0] == 0x03)
        {
            mRecv |= 0x400;
            vin[14] = data[1];
            vin[15] = data[2];
            vin[16] = data[3];
        }
        break;
    case 0x100b:
        mRecv |= 0x800;
        createFlowNo();
        connectorFlowNo = "";
        for(int i = 0; i < 6; i++)
        {
            connectorFlowNo += QString("%1").arg(data[i], 2, 10, QChar('0'));
        }
        qDebug() << "record flow" << connectorFlowNo;
        break;
    case 0x100c:
        mRecv |= 0x1000;
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
    default:
        break;
    }

    qDebug() << "record recv " << QString::number(mRecv, 16);
    if((mRecv & 0x1fff) == 0x1fff)
    {
        // send msg to pile
        mRecv = 0;
        CManage::getInstance()->sendMsgToPile(packer.packRecordQueryMsg(0x1009));

        handleVin();

        // check if the order exist...
        bool bret = CPileDataHelper::getInstance()->isOrderExisted(connectorCode, connectorFlowNo);

        qDebug() << "record order exist" << bret;

        if(bret == false)
        {
            // insert into database;
            saveOrder();

            // send msg to network
            CManage::getInstance()->sendRecordMsgToNetwork(connectorCode, flowNo, packOfflineOrderMsg());
        }
    }
}

void CRecordOrder::handleVin()
{
    bool bAll = true;

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

QByteArray CRecordOrder::packOfflineOrderMsg()
{
    QVariantMap param;
    param.insert("orderNo", flowNo);
    param.insert("operatorCode", CApp::getInstance()->getSEnterpriseCode());
    param.insert("stationCode", CApp::getInstance()->getSStationID());
    param.insert("equipmentCode", connectorCode.left(connectorCode.length() - 1));
    param.insert("connectorCode", connectorCode);
    param.insert("vin", vinStr);
    param.insert("startTime", startTime.toString("yyyy-MM-dd hh:mm:ss"));
    param.insert("startType", startType);
    param.insert("userChargeType", 2);
    param.insert("endTime", endTime.toString("yyyy-MM-dd hh:mm:ss"));
    param.insert("startSoc", startSoc);
    param.insert("endSoc", endSoc);
    param.insert("chargerDuration", duration);
    param.insert("stopType", stopType);
    param.insert("paymentAmount",  QString::number(amount, 'f', 2));
    param.insert("paymentAccount", "");
    param.insert("payChannel", 4);
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

    AESHandler handler;

    return handler.encodeMsg(data);
}

void CRecordOrder::saveOrder()
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

void CRecordOrder::createFlowNo()
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

    qDebug() << "flowno" << no;

    flowNo = no;
}

QString CRecordOrder::getConnectorCode() const
{
    return connectorCode;
}

void CRecordOrder::setConnectorCode(const QString &value)
{
    connectorCode = value;
}

QString CRecordOrder::getFlowNo() const
{
    return flowNo;
}

void CRecordOrder::setFlowNo(const QString &value)
{
    flowNo = value;
}

QString CRecordOrder::getUserChargeType() const
{
    return userChargeType;
}

void CRecordOrder::setUserChargeType(const QString &value)
{
    userChargeType = value;
}

QDateTime CRecordOrder::getStartTime() const
{
    return startTime;
}

void CRecordOrder::setStartTime(const QDateTime &value)
{
    startTime = value;
}

QDateTime CRecordOrder::getEndTime() const
{
    return endTime;
}

void CRecordOrder::setEndTime(const QDateTime &value)
{
    endTime = value;
}

int CRecordOrder::getStartSoc() const
{
    return startSoc;
}

void CRecordOrder::setStartSoc(int value)
{
    startSoc = value;
}

int CRecordOrder::getEndSoc() const
{
    return endSoc;
}

void CRecordOrder::setEndSoc(int value)
{
    endSoc = value;
}

int CRecordOrder::getDuration() const
{
    return duration;
}

void CRecordOrder::setDuration(int value)
{
    duration = value;
}

int CRecordOrder::getStopType() const
{
    return stopType;
}

void CRecordOrder::setStopType(int value)
{
    stopType = value;
}

float CRecordOrder::getPayment() const
{
    return payment;
}

void CRecordOrder::setPayment(float value)
{
    payment = value;
}

QString CRecordOrder::getPaymentAccount() const
{
    return paymentAccount;
}

void CRecordOrder::setPaymentAccount(const QString &value)
{
    paymentAccount = value;
}

QString CRecordOrder::getPaymentChannel() const
{
    return paymentChannel;
}

void CRecordOrder::setPaymentChannel(const QString &value)
{
    paymentChannel = value;
}

QString CRecordOrder::getCardNO() const
{
    return cardNO;
}

void CRecordOrder::setCardNO(const QString &value)
{
    cardNO = value;
}

float CRecordOrder::getAmount() const
{
    return amount;
}

void CRecordOrder::setAmount(float value)
{
    amount = value;
}

float CRecordOrder::getElecmoney() const
{
    return elecmoney;
}

void CRecordOrder::setElecmoney(float value)
{
    elecmoney = value;
}

float CRecordOrder::getQuantity() const
{
    return quantity;
}

void CRecordOrder::setQuantity(float value)
{
    quantity = value;
}

float CRecordOrder::getSharp() const
{
    return sharp;
}

void CRecordOrder::setSharp(float value)
{
    sharp = value;
}

float CRecordOrder::getPeak() const
{
    return peak;
}

void CRecordOrder::setPeak(float value)
{
    peak = value;
}

float CRecordOrder::getFlat() const
{
    return flat;
}

void CRecordOrder::setFlat(float value)
{
    flat = value;
}

float CRecordOrder::getValley() const
{
    return valley;
}

void CRecordOrder::setValley(const float &value)
{
    valley = value;
}

QString CRecordOrder::getVinStr() const
{
    return vinStr;
}

void CRecordOrder::setVinStr(const QString &value)
{
    vinStr = value;
}

QString CRecordOrder::getConnectorFlowNo() const
{
    return connectorFlowNo;
}

void CRecordOrder::setConnectorFlowNo(const QString &value)
{
    connectorFlowNo = value;
}

int CRecordOrder::getStartType() const
{
    return startType;
}

void CRecordOrder::setStartType(int value)
{
    startType = value;
}

float CRecordOrder::getStartmeter() const
{
    return startmeter;
}

float CRecordOrder::getEndmeter() const
{
    return endmeter;
}

#include "cverify.h"
#include "manage/capp.h"
#include "manage/cmanage.h"

CVerify::CVerify()
{
    init();
}

void CVerify::setConnectorCode(QString code)
{
    this->connectorCode = code;
}

void CVerify::init()
{
    memset(&cardNo, 0, sizeof(16));
    cardNoStr = "";
    balance = 0;
    mRecv = 0;
    flowNo = "";
    bCancleBook = 0;
    orderStartType = 0;
    bGetStartType = false;
    resetBookMsg();
}

void CVerify::reset()
{
    memset(&cardNo, 0, sizeof(16));
    cardNoStr = "";
    balance = 0;
    mRecv = 0;
    flowNo = "";
    orderStartType = 0;
    bCancleBook = 0;
    bGetStartType = false;
    resetBookMsg();
}

void CVerify::resetBookMsg()
{
    orderStartType = 0;
    orderTime = "";
    userChargeType = "";
    personalUserId = "";
    orderNo = "";
    plate = "";
    bCancleBook = 0;
}

bool CVerify::IsRecvCompleted()
{

    if((mRecv & 0x0f) == 0x0f)
    {
        return true;
    }
    else {
        return false;
    }
}

void CVerify::decodeCanMsg(int msgType, unsigned char *data)
{
    if(msgType == 0x0801)
    {
        if(data[0] == 0x01)
        {
            // 卡号
            mRecv |= 0x01;
            bGetStartType = true;
            if(data[2] == 0x00)
            {
                starttype = OPERATOR_CARD_START;
            }
            else
            {
                starttype = OTHER_START;
            }

            if(data[3] == 0x00)
            {
                permission = SUPER_USER;
            }
            else
            {
                permission = ORDINARY_USER;
            }

            if(starttype == OPERATOR_CARD_START)
            {
                cardNo[0] = data[4];
                cardNo[1] = data[5];
                cardNo[2] = data[6];
                cardNo[3] = data[7];
            }
            else if (starttype == OTHER_START) {
                userName[0] = data[4];
                userName[1] = data[5];
                userName[2] = data[6];
                userName[3] = data[7];
            }
        }
        else if(data[0] == 0x02)
        {
            mRecv |= 0x02;
            if(bGetStartType && (starttype == OPERATOR_CARD_START))
            {
                cardNo[4] = data[1];
                cardNo[5] = data[2];
                cardNo[6] = data[3];
                cardNo[7] = data[4];
                cardNo[8] = data[5];
                cardNo[9] = data[6];
                cardNo[10] = data[7];
            }
            else if (bGetStartType && (starttype == OTHER_START)) {
                userName[4] = data[1];
                userName[5] = data[2];
                userName[6] = data[3];
                userName[7] = data[4];
                userName[8] = data[5];
                userName[9] = data[6];
                userName[10] = data[7];
            }
        }
        else if(data[0] == 0x03)
        {
            mRecv |= 0x04;
            if(bGetStartType && (starttype == OPERATOR_CARD_START))
            {
                cardNo[11] = data[1];
                cardNo[12] = data[2];
                cardNo[13] = data[3];
                cardNo[14] = data[4];
                cardNo[15] = data[5];
            }
            else if (bGetStartType && (starttype == OTHER_START)) {
                userName[11] = data[1];
                userName[12] = data[2];
                userName[13] = data[3];
                userName[14] = data[4];
                userName[15] = data[5];
            }
        }
        else if(data[0] == 0x04)
        {
            mRecv |= 0x08;
            if(bGetStartType && (starttype == OTHER_START))
            {
                userPwd[0] = data[1];
                userPwd[1] = data[2];
                userPwd[2] = data[3];
                userPwd[3] = data[4];
                userPwd[4] = data[5];
                userPwd[5] = data[6];
            }
        }
    }
    else if(msgType == 0x0802)
    {
        mRecv |= 0x08;
        balance = (int)(((data[0] & 0xff) << 24) | ((data[1] & 0xff) << 16) | ((data[2] & 0xff) << 8) | ((data[3] & 0xff)));
    }
    else if (msgType == 0x0884) {
        // charger return the remote command status
        int command = data[0] & 0x0f;
        int res = (data[0] & 0x10) >> 4;

        CManage::getInstance()->sendMsgToMQ(packRemoteRetMsg(command, res), "remoteRet");
    }
    else if (msgType == 0x1884) {
        int command = data[0] & 0xff;
        int res = data[1] & 0xff;

        if((userChargeType.compare("3") == 0) || (userChargeType.compare("4") == 0))
        {
            if(command == 1)
            {
                CManage::getInstance()->sendBookOrderCreateMsg(packBookOrderMsg());
            }
            else if(command == 2)
            {
                CManage::getInstance()->sendBookOrderUpdateMsg(packBookCancleMsg());
                resetBookMsg();
            }
        }

        CManage::getInstance()->sendMsgToMQ(packBookingRetMsg(command, res), "bookRet");
    }

    if(IsRecvCompleted())
    {
        mRecv = 0;
        CManage::getInstance()->sendCardVerifyToNetwork(packVerifyMsg(), connectorCode, cardNoStr);
    }
}

/*
 * 创建预约订单,并且返回预约成功报文
*/
void CVerify::createBookOrder()
{
    if((userChargeType.compare("3") == 0) || (userChargeType.compare("4") == 0))
    {
        CManage::getInstance()->sendBookOrderCreateMsg(packBookOrderMsg());
    }

    CManage::getInstance()->sendMsgToMQ(packBookingRetMsg(1, 1), "bookRet");
}

/*
 * 取消预约订单
*/
void CVerify::cancleBookOrder()
{
    if((userChargeType.compare("3") == 0) || (userChargeType.compare("4") == 0))
    {
        CManage::getInstance()->sendBookOrderUpdateMsg(packBookCancleMsg());
    }

    CManage::getInstance()->sendMsgToMQ(packBookingRetMsg(2, 1), "bookRet");
}

/*
 * 后台现在只支持卡号验证
 * 暂时没有用户校验
*/
QByteArray CVerify::packVerifyMsg()
{
    QVariantMap param;
    param.insert("connectorCode", connectorCode);
    QString m_sCardNo = "";
    for(int i = 0; i < 4; i++)
    {
        //if(cardNo[i] != 0x00)
        {
            m_sCardNo += QString("%1").arg(cardNo[i], 2, 16, QChar('0'));
        }
    }

    //
    userChargeType = "2";

    cardNoStr = m_sCardNo;
    param.insert("cardNo", m_sCardNo);
    param.insert("stationCode", CApp::getInstance()->getSStationID());
    param.insert("currTime", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));

    QByteArray data = QtJson::serialize(param);

    AESHandler handler;
    return handler.encodeMsg(data);
}

/*
 * pack remote command return MSG_CMSG_CLOEXEC
*/
QByteArray CVerify::packRemoteRetMsg(int order, int res)
{
    QVariantMap param;
    param.insert("connectorCode", connectorCode);
    param.insert("dictate", order);
    if(res == 1)
    {
        param.insert("status", 1);
    }
    else
    {
        param.insert("status", 2);
    }
    param.insert("operatorCode", CApp::getInstance()->getSEnterpriseCode());
    param.insert("orderNo", flowNo);
    param.insert("userChargeType", userChargeType);
    //param.insert("orderStatus", 1);
    param.insert("startTime", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    param.insert("failReason", 0);
    param.insert("currTime", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    QByteArray data = QtJson::serialize(param);
    AESHandler handler;
    return handler.encodeMsg(data);
}

/*
 * pack booking return msg
*/
QByteArray CVerify::packBookingRetMsg(int order, int res)
{
    QVariantMap param;
    param.insert("connectorCode", connectorCode);
    param.insert("dictate", order);
    if(res == 1)
    {
        param.insert("status", 1);
    }
    else
    {
        param.insert("status", 2);
    }

    QVariantMap sub;
    sub.insert("userChargeType", userChargeType);
    sub.insert("orderNo", orderNo);
    sub.insert("personalUserId", personalUserId);
    sub.insert("bookingDate", orderTime);
    sub.insert("actualDate", orderTime);
    sub.insert("plate", "");
    sub.insert("park", "");
    param.insert("data", sub);

    param.insert("currTime", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    QByteArray data = QtJson::serialize(param);
    AESHandler handler;
    return handler.encodeMsg(data);
}

QByteArray CVerify::packBookCancleMsg()
{
    QVariantMap param;
    param.insert("orderNo", orderNo);

    // 计算超时时间，根据超时时间来计算超时费用
    QDateTime curDate = QDateTime::currentDateTime();
    QDateTime orderDate = QDateTime::fromString(orderTime, "yyyy-MM-dd hh:mm:ss");
    int deltaMin = curDate.secsTo(orderDate) / 60;
    int overTimeMin = overTime.toInt();
    if(deltaMin < overTimeMin){
        param.insert("isBac",0);
        param.insert("bacBalance", 0.0);
    }
    else{
        param.insert("isBac", 1);
        param.insert("bacBalance", 0.01);
    }

    //param.insert("data")

    param.insert("currTime", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    param.insert("plate", "");

    QByteArray data = QtJson::serialize(param);
    qDebug() << "pack order cancle msg" << data;
    AESHandler handler;

    return handler.encodeMsg(data);
}

QByteArray CVerify::packBookOrderMsg()
{
    orderNo = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
    QVariantMap param;
    param.insert("orderNo", orderNo);
    param.insert("personalUserId", personalUserId);
    param.insert("stationCode", CApp::getInstance()->getSStationID());
    param.insert("equipmentCode", connectorCode.left(connectorCode.length() - 1));
    param.insert("connectorCode", connectorCode);
    param.insert("bookingDate", orderTime);
    param.insert("actualDate", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    param.insert("currTime", QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    param.insert("userChargeType", userChargeType);
    param.insert("plate", plate);

    QByteArray data = QtJson::serialize(param);

    qDebug() << "pack book order " << data;
    AESHandler handler;
    return handler.encodeMsg(data);
}

QString CVerify::getCardNoStr() const
{
    return cardNoStr;
}

void CVerify::setCardNoStr(const QString &value)
{
    cardNoStr = value;
}

int CVerify::getBalance() const
{
    return balance;
}

void CVerify::setBalance(int value)
{
    balance = value;
}

int CVerify::getPermission() const
{
    return permission;
}

void CVerify::setPermission(int value)
{
    permission = (PERMISSION)value;
}

STARTTYPE CVerify::getStarttype() const
{
    return starttype;
}

QString CVerify::getOrderTime() const
{
    return orderTime;
}

void CVerify::setOrderTime(const QString &value)
{
    orderTime = value;
}

QString CVerify::getUserChargeType() const
{
    return userChargeType;
}

void CVerify::setUserChargeType(const QString &value)
{
    userChargeType = value;
}

QString CVerify::getPersonalUserId() const
{
    return personalUserId;
}

void CVerify::setPersonalUserId(const QString &value)
{
    personalUserId = value;
}

QString CVerify::getPlate() const
{
    return plate;
}

void CVerify::setPlate(const QString &value)
{
    plate = value;
}

int CVerify::getOrderStartType() const
{
    return orderStartType;
}

void CVerify::setOrderStartType(int value)
{
    orderStartType = value;
}

int CVerify::getBCancleBook() const
{
    return bCancleBook;
}

void CVerify::setBCancleBook(int value)
{
    bCancleBook = value;
}

QString CVerify::getOverTime() const
{
    return overTime;
}

void CVerify::setOverTime(const QString &value)
{
    overTime = value;
}

QString CVerify::getFlowNo() const
{
    return flowNo;
}

void CVerify::setFlowNo(const QString &value)
{
    flowNo = value;
}

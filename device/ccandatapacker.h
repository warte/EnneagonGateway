#ifndef CCANDATAPACKER_H
#define CCANDATAPACKER_H

#include <QByteArray>
#include <QSettings>
#include <QDateTime>

class CCANDataPakcer{
public:
    QByteArray packHeartbeatMsg(int status){
        QByteArray data;
        data.resize(12);
        data[0] = 0x08;
        data[1] = 0x05;
        data[2] = (uDevID & 0xff00) >> 8;
        data[3] = uDevID & 0xff;
        data[4] = status & 0xff;
        data[5] = 0x00;
        data[6] = 0x00;
        data[7] = 0x00;
        data[8] = 0x00;
        data[9] = 0x00;
        data[10] = 0x00;
        data[11] = 0x00;
        return data;
    }
    QByteArray packCardRetMsg(int status, int balance) {
        QByteArray data;
        data.resize(12);
        data[0] = 0x08;
        data[1] = 0x81;
        data[2] = (uDevID & 0xff00) >> 8;
        data[3] = uDevID & 0xff;
        switch (status) {
        case 200:
            data[4] = 0x01;
            break;
        case 506:
            data[4] = 0x02;
            break;
        case 500:
            data[4] = 0x04;
            break;
        default:
            data[4] = 0x02;
            break;
        }
        data[5] = (balance & 0xff000000) >> 24;
        data[6] = (balance & 0xff0000) >> 16;
        data[7] = (balance & 0xff00) >> 8;
        data[8] = balance & 0xff;
        data[9] = 0x00;
        data[10] = 0x00;
        data[11] = 0x00;
        return data;
    }
    QByteArray  packControlMsg(int controlType, int paramType, int param){
        QByteArray data;
        data.resize(12);
        data[0] = 0x08;
        data[1] = 0x04;
        data[2] = (uDevID & 0xff00) >> 8;
        data[3] = uDevID & 0xff;
        data[4] = controlType & 0xff;
        data[5] = paramType & 0xff;
        data[6] = (param & 0xff00) >> 8;
        data[7] = param & 0xff;
        data[8] = 0x00;
        data[9] = 0x00;
        data[10] = 0x00;
        data[11] = 0x00;
        return data;
    }
    QByteArray packBookingMsg(int command, int mode, int value){
        QByteArray data;
        data.resize(12);
        data[0] = 0x18;
        data[1] = 0x04;
        data[2] = (uDevID & 0xff00) >> 8;
        data[3] = uDevID & 0xff;
        data[4] = command & 0xff;
        data[5] = mode & 0xff;
        data[6] = (value & 0xff00) >> 8;
        data[7] = value & 0xff;
        data[8] = 0x00;
        data[9] = 0x00;
        data[10] = 0x00;
        data[11] = 0x00;
        return data;
    }
    QByteArray packBookingDateMsg(QString timestamp){
        QDateTime cur = QDateTime::fromString(timestamp, "yyyy-MM-dd hh:mm:ss");
        QByteArray data;
        data.resize(12);
        data[0] = 0x18;
        data[1] = 0x05;
        data[2] = (uDevID & 0xff00) >> 8;
        data[3] = uDevID & 0xff;
        data[4] = cur.date().year() - 2000;
        data[5] = cur.date().month();
        data[6] = cur.date().day();
        data[7] = cur.time().hour();
        data[8] = cur.time().minute();
        data[9] = cur.time().second();
        data[10] = 0x00;
        data[11] = 0x00;
        return data;
    }
    QByteArray packBookSettingMsg(int value, int symbol){
        QByteArray data;
        data.resize(12);
        data[0] = 0x18;
        data[1] = 0x06;
        data[2] = (uDevID & 0xff00) >> 8;
        data[3] = uDevID & 0xff;
        data[4] = (value >> 24) & 0xff;
        data[5] = (value >> 16) & 0xff;
        data[6] = (value >> 8) & 0xff;
        data[7] = value & 0xff;
        data[8] = symbol & 0xff;
        data[9] = 0x00;
        data[10] = 0x00;
        data[11] = 0x00;
        return data;
    }
    QByteArray packBookingAccount(int no, QByteArray account){
        QByteArray data;
        data.resize(12);
        data[0] = 0x18;
        data[1] = 0x07 + no;
        data[2] = (uDevID & 0xff00) >> 8;
        data[3] = uDevID & 0xff;
        if(account.size() <= (no * 8))
        {
            data[4] = 0x00;
        }
        else
        {
            data[4] = account[no * 8 + 0];
        }
        if(account.size() <= (no * 8 + 1))
        {
            data[5] = 0x00;
        }
        else {
            data[5] = account[no * 8 + 1];
        }
        if(account.size() <= (no * 8 + 2))
        {
            data[6] = 0x00;
        }
        else {
            data[6] = account[no * 8 + 2];
        }
        if(account.size() <= (no * 8 + 3))
        {
            data[7] = 0x00;
        }
        else {
            data[7] = account[no * 8 + 3];
        }
        if(account.size() <= (no * 8 + 4))
        {
            data[8] = 0x00;
        }
        else {
            data[8] = account[no * 8 + 4];
        }
        if(account.size() <= (no * 8 + 5))
        {
            data[9] = 0x00;
        }
        else {
            data[9] = account[no * 8 + 5];
        }
        if(account.size() <= (no * 8 + 6))
        {
            data[10] = 0x00;
        }
        else {
            data[10] = account[no * 8 + 6];
        }
        if(account.size() <= (no * 8 + 7))
        {
            data[11] = 0x00;
        }
        else {
            data[11] = account[no * 8 + 7];
        }
        return data;
    }
    QByteArray packStartTransactionMsg(int type, int res, int resReason, QString flowNo){
        QByteArray data;
        data.resize(12);
        data[0] = 0x08;
        data[1] = 0xd0;
        data[2] = (uDevID & 0xff00) >> 8;
        data[3] = uDevID & 0xff;
        int temp = 0;
        temp |= (resReason & 0x03) << 6;
        temp |= (res & 0x0f) << 4;
        temp |= type & 0x0f;
        data[4] = temp & 0xff;
        data[5] = flowNo.mid(0, 2).toInt() & 0xff;
        data[6] = flowNo.mid(2, 2).toInt() & 0xff;
        data[7] = flowNo.mid(4, 2).toInt() & 0xff;
        data[8] = flowNo.mid(6, 2).toInt() & 0xff;
        data[9] = flowNo.mid(8, 2).toInt() & 0xff;
        data[10] = flowNo.mid(10, 2).toInt() & 0xff;
        data[11] = 0x00;
        return data;
    }
    QByteArray packStopTransactionMsg(int type, int res, int resReason, int quantity, int amount){
        QByteArray data;
        data.resize(12);
        data[0] = 0x08;
        data[1] = 0xd0;
        data[2] = (uDevID & 0xff00) >> 8;
        data[3] = uDevID & 0xff;
        int temp = 0;
        temp |= (resReason & 0x03) << 6;
        temp |= (res & 0x0f) << 4;
        temp |= type & 0x0f;
        data[4] = temp & 0xff;
        data[5] = (quantity >> 16) & 0xff;
        data[6] = (quantity >> 8) & 0xff;
        data[7] = quantity & 0xff;
        data[8] = (amount >> 16) & 0xff;
        data[9] = (amount >> 8) & 0xff;
        data[10] = amount & 0xff;
        data[11] = 0x00;
        return data;
    }
    QByteArray packReturnMsg(ushort typeID, bool success){
        QByteArray data;
        data.resize(12);
        data[0] = (typeID & 0xff00) >> 8;
        data[1] = (typeID & 0xff) | 0x80;
        data[2] = (uDevID & 0xff00) >> 8;
        data[3] = uDevID & 0xff;
        if(success)
        {
            data[4] = 0xaa;
        }
        else {
            data[4] = 0x55;
        }
        data[5] = 0x00;
        data[6] = 0x00;
        data[7] = 0x00;
        data[8] = 0x00;
        data[9] = 0x00;
        data[10] = 0x00;
        data[11] = 0x00;
        return data;
    }
    QByteArray packSendMsg(ushort typeID, bool success)
    {
        QByteArray data;
        data.resize(12);
        data[0] = (typeID & 0xff00) >> 8;
        data[1] = (typeID & 0xff);
        data[2] = (uDevID & 0xff00) >> 8;
        data[3] = uDevID & 0xff;
        if(success)
        {
            data[4] = 0xaa;
        }
        else {
            data[4] = 0x55;
        }
        data[5] = 0x00;
        data[6] = 0x00;
        data[7] = 0x00;
        data[8] = 0x00;
        data[9] = 0x00;
        data[10] = 0x00;
        data[11] = 0x00;
        return data;
    }
    QByteArray packQueryStatusMsg(QString flowNo){
        QByteArray data;
        data.resize(12);
        data[0] = 0x08;
        data[1] = 0x51;
        data[2] = (uDevID & 0xff00) >> 8;
        data[3] = uDevID & 0xff;
        data[4] = flowNo.mid(0, 2).toInt() & 0xff;
        data[5] = flowNo.mid(2, 2).toInt() & 0xff;
        data[6] = flowNo.mid(4, 2).toInt() & 0xff;
        data[7] = flowNo.mid(6, 2).toInt() & 0xff;
        data[8] = flowNo.mid(8, 2).toInt() & 0xff;
        data[9] = flowNo.mid(10, 2).toInt() & 0xff;
        data[10] = 0x00;
        data[11] = 0x00;
        return data;
    }
    QByteArray packQueryConsumeMsg(QString flowNo){
        QByteArray data;
        data.resize(12);
        data[0] = 0x08;
        data[1] = 0x52;
        data[2] = (uDevID & 0xff00) >> 8;
        data[3] = uDevID & 0xff;
        data[4] = flowNo.mid(0, 2).toInt() & 0xff;
        data[5] = flowNo.mid(2, 2).toInt() & 0xff;
        data[6] = flowNo.mid(4, 2).toInt() & 0xff;
        data[7] = flowNo.mid(6, 2).toInt() & 0xff;
        data[8] = flowNo.mid(8, 2).toInt() & 0xff;
        data[9] = flowNo.mid(10, 2).toInt() & 0xff;
        data[10] = 0x00;
        data[11] = 0x00;
        return data;
    }
    QByteArray packSyncTimeMsg(QString timestamp){
        QDateTime cur = QDateTime::fromString(timestamp, "yyyy-MM-dd hh:mm:ss");
        QByteArray data;
        data.resize(12);
        data[0] = 0x08;
        data[1] = 0x00;
        data[2] = (uDevID & 0xff00) >> 8;
        data[3] = uDevID & 0xff;
        data[4] = cur.date().year() - 2000;
        data[5] = cur.date().month();
        data[6] = cur.date().day();
        data[7] = cur.time().hour();
        data[8] = cur.time().minute();
        data[9] = cur.time().second();
        data[10] = 0x00;
        data[11] = 0x00;
        return data;
    }
    QByteArray packElecPriceSettingMsg(int sharp, int peak, int flat, int valley){
        QByteArray data;
        data.resize(12);
        data[0] = 0x08;
        data[1] = 0x21;
        data[2] = (uDevID & 0xff00) >> 8;
        data[3] = uDevID & 0xff;
        data[4] = (sharp & 0xff00) >> 8;
        data[5] = sharp & 0xff;
        data[6] = (peak & 0xff00) >> 8;
        data[7] = peak & 0xff;
        data[8] = (flat & 0xff00) >> 8;
        data[9] = flat & 0xff;
        data[10] = (valley & 0xff00) >> 8;
        data[11] = valley & 0xff;
        return data;
    }
    QByteArray packRecordQueryMsg(ushort typeID){
        QByteArray data;
        data.resize(12);
        data[0] = (typeID & 0xff00) >> 8;
        data[1] = typeID & 0xff;
        data[2] = (uDevID & 0xff00) >> 8;
        data[3] = uDevID & 0xff;
        data[4] = 0x01;
        data[5] = 0x00;
        data[6] = 0x00;
        data[7] = 0x00;
        data[8] = 0x00;
        data[9] = 0x00;
        data[10] = 0x00;
        data[11] = 0x00;
        return data;
    }
    QByteArray packSuccessReturnMsg(ushort typeID){
        QByteArray data;
        data.resize(12);
        data[0] = (typeID & 0xff00) >> 8;
        data[1] = typeID & 0xff;
        data[2] = (uDevID & 0xff00) >> 8;
        data[3] = uDevID & 0xff;
        data[4] = 0x01;
        data[5] = 0x00;
        data[6] = 0x00;
        data[7] = 0x00;
        data[8] = 0x00;
        data[9] = 0x00;
        data[10] = 0x00;
        data[11] = 0x00;
        return data;
    }
    QByteArray packBookSuccessMsg(ushort typeID, int msgtype, int res){
        QByteArray data;
        data.resize(12);
        data[0] = (typeID & 0xff00) >> 8;
        data[1] = typeID & 0xff;
        data[2] = (uDevID & 0xff00) >> 8;
        data[3] = uDevID & 0xff;
        data[4] = msgtype & 0xff;
        data[5] = res & 0xff;
        data[6] = 0x00;
        data[7] = 0x00;
        data[8] = 0x00;
        data[9] = 0x00;
        data[10] = 0x00;
        data[11] = 0x00;
        return data;
    }
    QByteArray packElecPeriodSettingMsg(){
        return 0x00;
    }
    void setDevID(QString &connectorID);

    void setDevID(ushort devID);

private:
    ushort          uDevID;
};

#endif // CCANDATAPACKER_H


#ifndef CCARDMSG_H
#define CCARDMSG_H

#include <QString>
#include <QByteArray>
#include "device/ccandatapacker.h"
#include "cgundefine.h"

/*
 * 校验启动
 * 启动类型，
 * 卡号启动（超级卡，普通卡）
 * 用户启动（用户名，密码，用户权限）
 * 远程启动（用户名，远程控制的流水号判定）
*/

#include "utility/json.h"
#include <QDateTime>

class CVerify
{
public:
    CVerify();

    void            setConnectorCode(QString code);

    void            init();

    void            reset();

    void            resetBookMsg();

    bool            IsRecvCompleted();

    void            createBookOrder();

    void            cancleBookOrder();

    void            decodeCanMsg(int msgType, unsigned char* data);

    QByteArray      packVerifyMsg();

    QByteArray      packRemoteRetMsg(int order, int res);

    QByteArray      packBookingRetMsg(int order, int res);

    QByteArray      packBookOrderMsg();

    QByteArray      packBookCancleMsg();

    QString getCardNoStr() const;
    void setCardNoStr(const QString &value);

    int getBalance() const;
    void setBalance(int value);

    int getPermission() const;
    void setPermission(int value);

    STARTTYPE getStarttype() const;

    QString getOrderTime() const;
    void setOrderTime(const QString &value);

    QString getUserChargeType() const;
    void setUserChargeType(const QString &value);

    QString getPersonalUserId() const;
    void setPersonalUserId(const QString &value);

    QString getPlate() const;
    void setPlate(const QString &value);

    int getOrderStartType() const;
    void setOrderStartType(int value);

    int getBCancleBook() const;
    void setBCancleBook(int value);

    QString getOverTime() const;
    void setOverTime(const QString &value);

    QString getFlowNo() const;
    void setFlowNo(const QString &value);

private:
    QString         connectorCode;
    int             mRecv;
    bool            bGetStartType;
    unsigned char   cardNo[16];
    unsigned char   userName[16];
    unsigned char   userPwd[6];
    QString         cardNoStr;
    int             balance;
    PERMISSION      permission;
    STARTTYPE       starttype;
    // 远程启停的时候需要发送远程的流水号
    QString         flowNo;
    // book time
    int             orderStartType;
    QString         orderTime;
    QString         userChargeType;
    QString         personalUserId;
    QString         orderNo;
    QString         plate;
    int             bCancleBook;
    QString         overTime;
};

#endif // CCARDMSG_H

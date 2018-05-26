#ifndef CENNEAGONCONNECTOR_H
#define CENNEAGONCONNECTOR_H

#include <QObject>
#include "cgundefine.h"
#include "cverify.h"
#include "cconnectorstatus.h"
#include "cconnectororder.h"
#include "crecordorder.h"
#include "utility/aeshandler.h"

class CEnneagonConnector : public QObject
{
    Q_OBJECT
public:
    explicit CEnneagonConnector(QObject *parent = 0);

    void    init();

    void    showDetail();

    void    updateCounts();

    void    takeControl(int userChargeType, float vol,float cur, int mode, int value, QString account, int order);

    void    setRemoteOrderDetail(QString orderNo, QString phone);

    void    bookCharger(int userChargeType, QString bookdate,float vol,float cur, int mode, int value, QString account, int order, QString plate,QString overTime);

    void    updatePlatfromConsumeRetMsg(QString flowNo, bool ret);

    void    setStartType(const QString &userChargeType, const QString &account, const QString &channel);

    void    setCardNo(const QString &cardNO);

    void    updateStatus();

    CConnectorStatus status() const;

    CConnectorOrder order() const;

    CVerify verify() const;

    QByteArray  packChargingMsg();

    QByteArray  packRegisteMsg();

    void    readyToFinishOrder();

public:
    QString sChargerSn() const;
    void setSChargerSn(const QString &sChargerSn);

    QString sConnectorID() const;
    void setSConnectorID(const QString &sConnectorID);

    int dGun() const;
    void setDGun(int dGun);

    ushort dDevID() const;
    void setDDevID(const ushort &dDevID);

    QString sSoftwareVersion() const;
    void setSSoftwareVersion(const QString &sSoftwareVersion);

    QString sHardwareVersion() const;
    void setSHardwareVersion(const QString &sHardwareVersion);

    bool bIsReg() const;
    void setBIsReg(bool bIsReg);



public:
    void decode(int msgType, unsigned char *data);

    bool bSyncTime() const;

    bool bConnectedToPile() const;

private:
    QString         m_sChargerSn;
    QString         m_sConnectorID;
    int             m_dGun;
    ushort          m_dDevID;
    ChargerState    m_dState;
    bool            m_bConnectedToPile;
    int             m_dCounts;
    bool            m_bIsReg;
    bool            m_bSyncTime;
    QString         m_sSoftwareVersion;
    QString         m_sHardwareVersion;

    bool            m_bStartBook;
    bool            m_bBookOverTime;

    // 卡号用户类
    CVerify         mVerify;

    // 充电枪状态类
    CConnectorStatus mStatus;

    // 订单类
    CConnectorOrder mOrder;

    // record order
    CRecordOrder    mRecordOrder;

signals:

public slots:
    void        completeOrder();

    void        checkOverBookOrder();
};

#endif // CENNEAGONCONNECTOR_H

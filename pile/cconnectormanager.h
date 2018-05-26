#ifndef CCONNECTORMANAGER_H
#define CCONNECTORMANAGER_H

#include <QObject>
#include <QList>
#include <QSqlRecord>
#include <QTimer>
#include "cenneagonconnector.h"
#include "utility/cdatahelper.h"
#include "cbasewarning.h"

class CConnectorManager : public QObject
{
    Q_OBJECT
public:
    explicit CConnectorManager(QObject *parent = 0);

    /*
     * 初始化，从数据库中查询所有的充电枪（按照每个桩的枪数来创建）
    */
    void    init();
    /*
     * 读取数据库内容
    */
    void    getSysPileMsg(QSqlRecord rec);
    /*
     * 读取数据库设定的告警信息
    */
    void    getSysWarning(QSqlRecord rec);
    /*
     *
    */
    void    registeSystem();
    /*
     *
    */
    void    pushHeartBeatMsg();
    /*
     *
    */
    bool    getRegisteStatus();
    /*
     * 获取后台发送的激活数据
    */
    void    getRegisteActivateMsg(QString sn, bool bactive);
    /*
     * 获取后台发送的订单创建状态
    */
    void    getPlatformFlowRetMsg(QString connectorID, bool bret);
    /*
     *  get pcard verify return msg from platfrom
    */
    void    getCardVerifyRetMsg(const QString &connectorID, int status, const QString &cardNo);
    /*
     * get platfrom consume status
    */
    void    getPlatformConsumeRetMsg(QString connectorID, QString flowNo, bool bret);
    /*
     *
    */
    void    handleRemoteControlCommand(QString connectorID, int userChargeType, float vol,float cur, int mode, int value, QString account, int order);
    /*
     *
    */
    void    handleRemoteBookingCommand(QString connectorID, int userChargeType, QString bookdate,float vol,float cur, int mode, int value, QString account, int order, QString plate, QString overtime);
    /*
     * 设置流水号以及手机号
    */
    void    setRemoteDetail(QString connectorID, QString orderNo, QString phone);
    /*
     * sync time with charger
    */
    void    syncChargerTime(QString timestamp);

    /*
     * set ready to completed order
    */
    void    setReadyToCompleteOrder(const QString &code);
    /*
     * 处理CAN报文
    */
    void    handleCanMsg(QByteArray data);
    /*
     * 获取设备ID
    */
    ushort getDevIDByConnetorID(const QString& connectorID);
    /*
     *
    */
    QByteArray  getChargingMsgByCode(const QString& connectorID);
    /*
     * heartbeat msg to server
     * get all chargers's status.online or offline
    */
    QByteArray packHeartBeatMsg();
    /*
     * send charging msg
    */
    void    sendChargingMsg(const QString &connectorcode);

public:
    static CConnectorManager*   mInstance;
    static CConnectorManager*   getInstance();

    bool getBOnline() const;
    void setBOnline(bool bOnline);

    QList<CEnneagonConnector *> getConnectorList() const;

private:
    QList<CEnneagonConnector*>  m_connectorList;

    bool    m_bOnline;

    bool    m_bGetMsg;

    int     currIndex;

    QTimer  *mTimer;

signals:

public slots:
    void    handleTimeout();
};

#endif // CCONNECTORMANAGER_H

#ifndef CMANAGE_H
#define CMANAGE_H

/*
 * 管理充电桩或者车辆的创建
 * 管理设备的创建
 * 管理通信
 * 管理程序运行
*/

#include <QObject>
#include <QList>
#include <QMap>
#include <QSettings>
#include <QProcess>
#include <QTimer>
#include <QMutex>
#include <QMutexLocker>
#include "device/canworkerthread.h"
#include "device/canwrapper.h"

#include "pile/cenneagonconnector.h"
#include "capp.h"
#include "pile/cconnectormanager.h"

#include "utility/cdatahelper.h"
#include "utility/json.h"
#include "utility/cdatauploader.h"
/*
 * MQ
*/
#include "network/crabbitmq.h"
/*
 * TCP
*/
#include "network/cremotesocket.h"
/*
 * http接口
*/
#include "network/creghttp.h"
#include "network/ccardverifyhttphandler.h"
#include "network/cstatehttp.h"
#include "network/cflownohttp.h"
#include "network/cconsumehttp.h"
#include "network/crecordhttp.h"
#include "network/cbookorderhttp.h"
#include "network/cbookorderupdatehttp.h"
#include "network/calirefund.h"
#include "network/cwechatrefund.h"

class CManage : public QObject
{
    Q_OBJECT
public:
    explicit CManage(QObject *parent = 0);

    /*
     * 读取配置
    */
    void    readDefaultSetting();
    /*
     * 初始化设备，暂时是can和串口
    */
    void    initDevice();
    /*
     * 关闭设备
    */
    void    closeDevice();
    /*
     * 初始化网络
    */
    void    initNetwork();
    /*
     * 关闭网络
    */
    void    closeNetwork();
    void    resetMq();

    /*
     * 初始化信号槽函数
    */
    void    initConnection();
    /*
     * 开始系统
    */
    void    startSystem();
    /*
     * 注册设备
    */
    void    registeSystem();
    /*
     * 初始化定时器
    */
    void    initTimer();

    /*
     * 网络调用接口，包括CAN数据的发送
     * 流水号，卡号信息查询，结算报文，注册激活报文
     * 状态信息，告警信息上送
    */
public:
    // 发送数据到充电桩接口
    void    sendMsgToPile(const QByteArray& data);
    // 更新MQ的连接属性
    void    updateMQConnectionInfo(QString &host, QString port, QString &user, QString &pwd);
    // send msg to mq
    void    sendMsgToMQ(const QByteArray &msg, const QString &type);
    // 流水号请求报文
    void    sendFlowNoRequest(QString connectorID, const QString& localFlow, const QByteArray& reqdata);

    // 请求激活报文接口
    void    sendRegisteReqMsgToNetwork(const QByteArray& data);

    // 发送卡号验证报文
    void    sendCardVerifyToNetwork(const QByteArray& data, const QString& connectorID, const QString &cardNO);

    // 状态变更报文
    void    sendChargerStateChangeToNetwork(const QByteArray& data, const QString& connectorID);

    // send consume msg to platform
    void    sendConsumeMsgToNetwork(const QString &connectorID, const QString &flow, const QByteArray &data);

    // send record msg to platform
    void    sendRecordMsgToNetwork(const QString &connectorID, const QString &flow, const QByteArray &data);

    // 创建预约订单
    void    sendBookOrderCreateMsg(const QByteArray& data);

    // 更新预约订单
    void    sendBookOrderUpdateMsg(const QByteArray& data);

    // 微信退款
    void    sendWechatRefund(const QByteArray& data);

    // 支付宝退款
    void    sendAliRefund(const QByteArray& data);

    // 整理心跳报文
    QByteArray  packHeartBeatMsg();
    //
    QByteArray  packHeartbeatDetailMsg();

public:
    static CManage*   mInstance;
    static CManage*   getInstance();

private:
    /*
     *  can 设备
    */
    CanWorkerThread*    m_canThread;
    CanWrapper*         m_canWrapper;

    CanWorkerThread*    m_canThread1;//CAN1
    CanWrapper*         m_canWrapper1;//CAN1

    QMutex          mutex;

    /*
     * MQ
    */
    CRabbitMQ       *m_rabbitMQ;

    CRemoteSocket   *m_sock;

    /*
     * 数据发送定时器
    */
    QTimer*         m_regTimer;

signals:

public slots:
    // 分发CAN报文到各个充电桩
    void    setData(QByteArray data);
    /*
     * 发送数据
    */
    void    keepRegTimeout();
};

#endif // CMANAGE_H

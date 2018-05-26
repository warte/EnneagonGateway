#ifndef CCONNECTORSTATUS_H
#define CCONNECTORSTATUS_H

#include "utility/json.h"
#include "cgundefine.h"
#include "utility/cdatahelper.h"
#include "cbasewarning.h"
#include <QSqlRecord>

class CConnectorStatus
{
public:
    CConnectorStatus();

    void    setConnectorCode(QString code);

    void    init();

    void    resetParam();

    void    decodeCanMsg(int msgType, unsigned char* data);

    void    handleBMSMsg(int msgType, unsigned char* data);

    void    handleHeartBeatMsg(unsigned char *data);

    void    getSysErrCode(QSqlRecord rec);

    bool    isStateChange();

    void    sendStatusToNetwork();

    bool    isAlarm();

    /*
     * 打包状态变更数据
    */
    QByteArray  packStateChangeMsg();

    /*
     *
    */
    QByteArray  packChargeOrdinaryMsg();

    void    setOfflineState();

    int getChargerState() const;

    int getConnectorState() const;

    int getLockState() const;

    double getFVoltage() const;

    double getFCurrent() const;

    int getDStartSoc() const;

    int getDEndSoc() const;

    int getDCurrentSoc() const;

    int getDConnectorTemp() const;

    int getDRemainTime() const;

    double getFAmount() const;

    int getDChargingTime() const;

    double getFQuantity() const;

    BMSPROTOCOL getBmsProtocol() const;

    int getDBatteryType() const;

    float getFBatteryCapacity() const;

    float getFBatteryRatedVoltage() const;

    float getFBatteryRatedPower() const;

    float getFHighestAllowChargeVoltage() const;

    float getFHighestAllowChargeCurrent() const;

    float getFCellCutoffVoltage() const;

    int getDMotiveBatteryStartSoc() const;

    int getDHighestAllowTemperature() const;

    float getFBmsReqVoltage() const;

    float getFBmsReqCurrent() const;

    float getFCellHighestVoltage() const;

    int getDCellHighestVoltageGroup() const;

    int getDBatteryHighestTemperature() const;

    int getDBatteryLowestTemperature() const;

    CHARGETYPE getChargeType() const;

    int getDCellHighestVoltageNO() const;

    int getDHighestTempNo() const;

    int getDLowestTempNo() const;

    float getFACVoltageA() const;

    float getFACVoltageB() const;

    float getFACVoltageC() const;

    float getFACCurrentA() const;

    float getFACCurrentB() const;

    float getFACCurrentC() const;

    QList<CBaseWarning *> getBmsWarning() const;

    QList<CBaseWarning *> getConnectorWarning() const;

    float getFSharp() const;

    float getFPeak() const;

    float getFFlat() const;

    float getFValley() const;    
private:
    int     mRecv;
    QString connectorCode;
    // 用来判断多长时间没有收到充电桩的心跳
    // 超过30秒判离线
    int     lostCounts;
    //  充电枪状态
    int     chargerState;
    // 连接器状态
    int     connectorState;
    // 电子锁状态
    int     lockState;
    /*
     * 充电桩相关数据
    */
    double  m_fVoltage;
    double  m_fCurrent;
    int     m_dStartSoc;
    bool    bSocSet;
    int     m_dEndSoc;
    int     m_dCurrentSoc;
    int     m_dConnectorTemp;
    int     m_dRemainTime;
    double  m_fAmount;
    int     m_dChargingTime;
    double  m_fQuantity;
    float   m_fSharp;
    float   m_fPeak;
    float   m_fFlat;
    float   m_fValley;

    int     moduleCount;
    /*
     * BMS 相关信息
    */
    // bms版本号
    BMSPROTOCOL bmsProtocol;
    // 电池类型
    int         m_dBatteryType;
    // 电池额定容量
    float       m_fBatteryCapacity;
    // 电池额定总压
    float       m_fBatteryRatedVoltage;
    // 电池标称总能量
    float       m_fBatteryRatedPower;
    // 最高允许充电总压
    float       m_fHighestAllowChargeVoltage;
    // 最高允许充电电流
    float       m_fHighestAllowChargeCurrent;
    // 单体截止电压
    float       m_fCellCutoffVoltage;
    // 动力蓄电池开始soc
    int         m_dMotiveBatteryStartSoc;
    // 电池最高允许温度
    int         m_dHighestAllowTemperature;
    // 电压需求
    float       m_fBmsReqVoltage;
    // 电流需求
    float       m_fBmsReqCurrent;
    // 单体最高电压
    float       m_fCellHighestVoltage;
    // 单体最高电压组号
    int         m_dCellHighestVoltageGroup;
    // 电池最高温度
    int         m_dBatteryHighestTemperature;
    // 电池最低温度
    int         m_dBatteryLowestTemperature;
    // 充电阶段
    CHARGETYPE  chargeType;
    // 单体最高电压所在编号
    int         m_dCellHighestVoltageNO;
    // 最高温度监测点编号
    int         m_dHighestTempNo;
    // 最低温度监测点编号
    int         m_dLowestTempNo;

    /*
     * 交流进线信息
    */
    // A相交流电压
    float       m_fACVoltageA;
    // B相交流电压
    float       m_fACVoltageB;
    // C相交流电压
    float       m_fACVoltageC;
    // A相交流电流
    float       m_fACCurrentA;
    // B相交流电流
    float       m_fACCurrentB;
    // C相交流电流
    float       m_fACCurrentC;

    /*
     * BMS告警信息
    */
    QList<CBaseWarning*>    m_bmsWarning;

    /*
     * 充电机告警信息
    */
    QList<CBaseWarning*>    m_connectorWarning;
};

#endif // CCONNECTORSTATUS_H

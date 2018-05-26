#ifndef CGUNDEFINE_H
#define CGUNDEFINE_H

#include <QMap>
#include <QByteArray>
#include <QDateTime>

enum ChargerState{
    STANBY = 1,
    CHARGING = 2,
    CHARGED = 3,
    CHARGERERROR = 4,
    CHARGERBOOKING = 5,
    CHARGEROFFLINE = 6
};

enum STARTTYPE{
    OPERATOR_APP_START = 1,
    OPERATOR_CARD_START = 2,
    PERSON_APP_START = 3,
    WECHART_START = 4,
    PLATFORM_REMOTE = 5,
    OTHER_START = 6
};

enum PERMISSION{
    SUPER_USER = 1,
    ORDINARY_USER = 2
};

enum BATTERYTYPE{
    QIANSUAN_BATTERY = 1,
    NIEQING_BATTERY = 2,
    LINSUANTIELI_BATTERY = 3,
    MENGSUANLI_BATTERY = 4,
    GUSUANLI_BATTERY = 5,
    SANYUANCAILIAO_BATTERY = 6,
    JUHEWULILIZI_BATTERY = 7,
    TAISUANLI_BATTERY = 8,
    OTHER = 9
};

enum BMSPROTOCOL{
    BMS_2011 = 1,
    BMS_2015 = 2
};

enum CHARGETYPE{
    CONSTANT_VOLTAGE = 1,
    CONSTANT_CURRENT = 2
};

struct _RemoteControl{
    bool    bDataCompleted;
    bool    bSuccess;
    int     dOperationType;
    int     dReason;
    int     dChargeType;
    int     dData;
};

struct _PILESTATUS{
    // 抢口连接状态
    int connectotState;
    // 电子锁状态
    int lockState;
    // 通信状态
    int communicationState;
    // 使用状态
    int useState;
};


struct _Warning{
    int     dDataCompleted;
    int     pile;
    int     gunA;
    int     gunB;
    int     moduleCount;
};

struct _CARDVERIFY{
    int     dDataCompleted;
    unsigned char cardNo[16];

};

struct _ONOFFCOMMAND{
    //int     dDataCompleted;
    int     commandType;
    //unsigned char vin[17];
};

struct _VIN_{
    int dDataCompleted;
    unsigned char vin[17];
};

struct _Status_{
    //
    bool    bDataCompleted;
    //
    int     dDataCompleted;
    //  充电枪状态
    int     chargerState;
    // 链接状态
    int     connectorState;
    /*
     * 电压电流
    */
    double   chargingVoltage;
    double   chargingCurrent;
    int     startSoc;
    int     endSoc;
    int     currentSoc;
    bool    bSetSoc;
    int     remainTime;
    double   consumeAmount;
    int     chargingTime;
    double   chargingPower;
    int     power;
    int     moduleCount;
    /*
     * 额定电压,单体最高电压，单体截止电压，电池当前电压
    */
    double   bmsReqVoltage;
    double   bmsReqCurrent;
    double   cellHighestVoltage;
    double   cellStopVoltage;

    double   allowVoltage;
    int     cellHighestTemp;
    int     cellLowestTemp;

    double  batteryVoltage;
    double  batteryCurrent;

    /*
     * 告警
    */
    QMap<QString,int> warningMsg;

    QByteArray vin;

    QByteArray dealSerialNum;
};


struct _RATE_{
    int dDataCompleted;
    // 开始电表总电度
    double   startMeterPower;
    // 结束电表总电度
    double   endMeterPower;
    //尖总电度
    double   sharpQuantity;
    //峰总电度
    double   peakQuantity;
    // 平总电度
    double   flatQuantity;
    // 谷总电度
    double   valleyQuantity;
    // 尖开始总电度
    double   sharpStartQuantity;
    // 尖结束总电度
    double   sharpEndQuantity;
    // 尖消费金额
    double   sharpConsumeAmount;
    // 峰开始总电度
    double   peakStartQuantity;
    // 峰开始总电度
    double   peakEndQuantity;
    // 峰消费金额
    double   peakConsumeAmount;
    // 平开始总电度
    double   flatStartQuantity;
    // 平结束总电度
    double   flatEndQuantity;
    // 平消费金额
    double   flatConsumeAmount;
    //谷开始总电度
    double   valleyStartQuantity;
    // 谷结束总电度
    double   valleyEndQuantity;
    // 谷消费金额
    double   valleyConsumeAmount;
};

#endif // CGUNDEFINE_H

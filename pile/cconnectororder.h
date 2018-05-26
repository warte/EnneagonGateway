#ifndef CCONNECTORORDER_H
#define CCONNECTORORDER_H

#include <QDateTime>
#include <QTimer>

class CConnectorOrder
{

public:
    explicit CConnectorOrder();

    void    createFlowNo();

    void    decodeCanMsg(int msgType, unsigned char *data);

    void    consumeOrder();

    bool    isOrderNeedToConsume(int state);

    QByteArray  packOrderCreateMsg();

    void    saveOrder();

    void    resetParam();

    void    resetMainParam();

    QByteArray  packOrderMsg();

    QByteArray  packWechatRefund();

    QByteArray  packAliRefund();

    void    handleVin();

    QString getConnectorCode() const;
    void setConnectorCode(const QString &value);

    QString getFlowNo() const;
    void setFlowNo(const QString &value);

    QString getUserChargeType() const;
    void setUserChargeType(const QString &value);

    QDateTime getStartTime() const;
    void setStartTime(const QDateTime &value);

    QDateTime getEndTime() const;
    void setEndTime(const QDateTime &value);

    int getStartSoc() const;
    void setStartSoc(int value);

    int getEndSoc() const;
    void setEndSoc(int value);

    int getDuration() const;
    void setDuration(int value);

    int getStopType() const;
    void setStopType(int value);

    int getPayment() const;
    void setPayment(int value);

    QString getPaymentAccount() const;
    void setPaymentAccount(const QString &value);

    QString getPaymentChannel() const;
    void setPaymentChannel(const QString &value);

    QString getCardNO() const;
    void setCardNO(const QString &value);

    float getAmount() const;
    void setAmount(float value);

    float getElecmoney() const;
    void setElecmoney(float value);

    float getQuantity() const;
    void setQuantity(float value);

    float getSharp() const;
    void setSharp(float value);

    float getPeak() const;
    void setPeak(float value);

    float getFlat() const;
    void setFlat(float value);

    float getValley() const;
    void setValley(const float &value);

    QString getVinStr() const;
    void setVinStr(const QString &value);

    QString getConnectorFlowNo() const;
    void setConnectorFlowNo(const QString &value);

    int getStartType() const;
    void setStartType(int value);

    bool getBCreateSuccess() const;
    void setBCreateSuccess(bool value);

    bool getBStartCharging() const;
    void setBStartCharging(bool value);

    bool getBConsume() const;
    void setBConsume(bool value);

    float getStartmeter() const;

    float getEndmeter() const;

    void setStartmeter(float value);

    void setEndmeter(float value);

    QString getPhoneNo() const;
    void setPhoneNo(const QString &value);

private:
    int mRecv;
    int m_consumeRecv;
    QString connectorCode;
    QString flowNo;
    QString connectorFlowNo;

    QString phoneNo;

    /*
     * if the platfrom order create success
    */
    bool    bCreateSuccess;

    bool    bStartCharging;

    /*
     * platfrom return the consume result;
     * if return true,
    */
    bool    bConsume;

    QString userChargeType;
    QDateTime   startTime;
    QDateTime   endTime;
    QString vinStr;
    int     startType;
    unsigned char vin[17];
    int     startSoc;
    int     endSoc;
    int     duration;
    float   startmeter;
    float   endmeter;
    int     stopType;
    int     payment;
    QString paymentAccount;
    QString paymentChannel;
    QString cardNO;


    /*
     * 消费总金额
    */
    float   amount;
    float   elecmoney;
    float   servicemoney;
    float   quantity;
    float   sharp;
    float   peak;
    float   flat;
    float    valley;
};

#endif // CCONNECTORORDER_H

#ifndef CBASEWARNING_H
#define CBASEWARNING_H

/*
 * 告警基础类
*/
#include <QDateTime>
#include "utility/cdatahelper.h"

class CBaseWarning
{
public:
    CBaseWarning();

    QString getConnectorCode() const;
    void setConnectorCode(const QString &value);

    QString getName() const;
    void setName(const QString &value);

    QString getName_cn() const;
    void setName_cn(const QString &value);

    QString getName_en() const;
    void setName_en(const QString &value);

    int getByte() const;
    void setByte(int value);

    int getStartBit() const;
    void setStartBit(int value);

    int getLen() const;
    void setLen(int value);

    int getValue() const;
    void setValue(int value);

    int getCode() const;
    void setCode(int value);

    QString getAlarmCode() const;
    void setAlarmCode(const QString &value);

    int getAlarmType() const;
    void setAlarmType(int value);

    bool getBWarn() const;
    void setBWarn(bool value);

    void        insertWarningMsg();

private:
    QString     connectorCode;
    QString     name;
    QString     name_cn;
    QString     name_en;
    int         byte;
    int         startBit;
    int         len;
    int         value;
    int         code;
    int         alarmType;
    QString     alarmCode;

    bool        bWarn;
    QDateTime   startTime;
    QDateTime   stopTime;
};

#endif // CBASEWARNING_H

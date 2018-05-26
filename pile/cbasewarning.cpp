#include "cbasewarning.h"

CBaseWarning::CBaseWarning()
{
    bWarn = false;
}

QString CBaseWarning::getConnectorCode() const
{
    return connectorCode;
}

void CBaseWarning::setConnectorCode(const QString &value)
{
    connectorCode = value;
}

QString CBaseWarning::getName() const
{
    return name;
}

void CBaseWarning::setName(const QString &value)
{
    name = value;
}

QString CBaseWarning::getName_cn() const
{
    return name_cn;
}

void CBaseWarning::setName_cn(const QString &value)
{
    name_cn = value;
}

QString CBaseWarning::getName_en() const
{
    return name_en;
}

void CBaseWarning::setName_en(const QString &value)
{
    name_en = value;
}

int CBaseWarning::getByte() const
{
    return byte;
}

void CBaseWarning::setByte(int value)
{
    byte = value;
}

int CBaseWarning::getStartBit() const
{
    return startBit;
}

void CBaseWarning::setStartBit(int value)
{
    startBit = value;
}

int CBaseWarning::getLen() const
{
    return len;
}

void CBaseWarning::setLen(int value)
{
    len = value;
}

int CBaseWarning::getValue() const
{
    return value;
}

void CBaseWarning::setValue(int value)
{
    if(value != 0)
    {
        if(bWarn == false)
        {
            bWarn = true;
            startTime = QDateTime::currentDateTime();
        }
    }
    else {
        if(bWarn == true)
        {
            bWarn = false;
            stopTime = QDateTime::currentDateTime();

            insertWarningMsg();
        }
    }

    value = value;
}

void CBaseWarning::insertWarningMsg()
{
    CPileDataHelper::getInstance()->addWarningMsg(connectorCode, alarmCode, startTime.toString("yyyy-MM-dd hh:mm:ss"), stopTime.toString("yyyy-MM-dd hh:mm:ss"));
}

int CBaseWarning::getCode() const
{
    return code;
}

void CBaseWarning::setCode(int value)
{
    code = value;
}

QString CBaseWarning::getAlarmCode() const
{
    return alarmCode;
}

void CBaseWarning::setAlarmCode(const QString &value)
{
    alarmCode = value;
}

int CBaseWarning::getAlarmType() const
{
    return alarmType;
}

void CBaseWarning::setAlarmType(int value)
{
    alarmType = value;
}

bool CBaseWarning::getBWarn() const
{
    return bWarn;
}

void CBaseWarning::setBWarn(bool value)
{
    bWarn = value;
}

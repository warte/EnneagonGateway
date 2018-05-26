#ifndef CMODULE_H
#define CMODULE_H

#include <QString>

class CModule
{
public:
    CModule();

    QString sConnectorCode() const;
    void setSConnectorCode(const QString &sConnectorCode);

    int dModuleNo() const;
    void setDModuleNo(int dModuleNo);

    float fModuleVoltage() const;
    void setFModuleVoltage(float fModuleVoltage);

    float fModuleCurrent() const;
    void setFModuleCurrent(float fModuleCurrent);

    int dModuleTemperature() const;
    void setDModuleTemperature(int dModuleTemperature);

private:
    QString m_sConnectorCode;
    int     m_dModuleNo;
    float   m_fModuleVoltage;
    float   m_fModuleCurrent;
    int     m_dModuleTemperature;
};

#endif // CMODULE_H

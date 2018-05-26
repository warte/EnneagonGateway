#include "cmodule.h"

CModule::CModule()
{

}

QString CModule::sConnectorCode() const
{
    return m_sConnectorCode;
}

void CModule::setSConnectorCode(const QString &sConnectorCode)
{
    m_sConnectorCode = sConnectorCode;
}

int CModule::dModuleNo() const
{
    return m_dModuleNo;
}

void CModule::setDModuleNo(int dModuleNo)
{
    m_dModuleNo = dModuleNo;
}

float CModule::fModuleVoltage() const
{
    return m_fModuleVoltage;
}

void CModule::setFModuleVoltage(float fModuleVoltage)
{
    m_fModuleVoltage = fModuleVoltage;
}

float CModule::fModuleCurrent() const
{
    return m_fModuleCurrent;
}

void CModule::setFModuleCurrent(float fModuleCurrent)
{
    m_fModuleCurrent = fModuleCurrent;
}

int CModule::dModuleTemperature() const
{
    return m_dModuleTemperature;
}

void CModule::setDModuleTemperature(int dModuleTemperature)
{
    m_dModuleTemperature = dModuleTemperature;
}

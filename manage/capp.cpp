#include "capp.h"
#include <string>
CApp* CApp::mInstance = NULL;

CApp::CApp(QObject *parent) : QObject(parent)
{
    m_dDeltaTime = 1000;
}

CApp* CApp::getInstance()
{
    if(!mInstance)
    {
        mInstance = new CApp();
    }

    return mInstance;
}

void CApp::init()
{
    CPileDataHelper::getInstance()->queryDefaultSetting();

    CPileDataHelper::getInstance()->queryRateSetting();
}

void CApp::getSysSetting(QSqlRecord rec)
{
    m_sStationID = rec.value(rec.indexOf("stationID")).toString();
    m_sEnterpriseCode = rec.value(rec.indexOf("enterpriseCode")).toString();
    m_dDeltaTime = rec.value(rec.indexOf("deltaTime")).toInt();
    if(m_dDeltaTime < 100)
    {
        m_dDeltaTime = 1000;
    }
    m_sBaseUrl = rec.value(rec.indexOf("httpUrl")).toString();
    m_sMqUrl = rec.value(rec.indexOf("mqUrl")).toString();
    m_sMqPort = rec.value(rec.indexOf("mqPort")).toString();
    m_sMqUser = rec.value(rec.indexOf("mqUser")).toString();
    m_sMqPassword = rec.value(rec.indexOf("mqPassword")).toString();
    m_sRemoteIP = rec.value(rec.indexOf("remoteIP")).toString();
    m_dRemotePort = rec.value(rec.indexOf("remotePort")).toInt();
    m_sGatewaySn = rec.value(rec.indexOf("deviceName")).toString();
    m_sSecrteCode = rec.value(rec.indexOf("secretKey")).toString();
    m_bSecretCode = rec.value(rec.indexOf("secretKey")).toByteArray();
    m_key = std::string(m_bSecretCode.constData(), m_bSecretCode.length());
    m_sSecretVector = rec.value(rec.indexOf("secretIv")).toString();
    m_bSecretVector = rec.value(rec.indexOf("secretIv")).toByteArray();
    m_iv = std::string(m_bSecretVector.constData(), m_bSecretVector.length());
}

void CApp::getRateSetting(QSqlRecord rec)
{
    m_sharpRate = rec.value(rec.indexOf("sharpRate")).toFloat();
    m_sharpService = rec.value(rec.indexOf("sharpService")).toFloat();
    m_peakRate = rec.value(rec.indexOf("peakRate")).toFloat();
    m_peakService = rec.value(rec.indexOf("peakService")).toFloat();
    m_flatRate = rec.value(rec.indexOf("flatRate")).toFloat();
    m_flatService = rec.value(rec.indexOf("flatService")).toFloat();
    m_valleyRate = rec.value(rec.indexOf("valleyRate")).toFloat();
    m_valleyService = rec.value(rec.indexOf("valleyService")).toFloat();
}

bool CApp::isMqStandSame(QString url, QString port, QString user, QString pwd)
{
    if((m_sMqUrl.compare(url) == 0) && (m_sMqPort.compare(port) == 0) && (m_sMqUser.compare(user) == 0) && (m_sMqPassword.compare(pwd) == 0))
    {
        return true;
    }
    else
    {
        return false;
    }
}

QString CApp::getSStationID() const
{
    return m_sStationID;
}

void CApp::setSStationID(const QString &sStationID)
{
    m_sStationID = sStationID;
}

QString CApp::getSEnterpriseCode() const
{
    return m_sEnterpriseCode;
}

void CApp::setSEnterpriseCode(const QString &sEnterpriseCode)
{
    m_sEnterpriseCode = sEnterpriseCode;
}

QString CApp::getSSecrteCode() const
{
    return m_sSecrteCode;
}

void CApp::setSSecrteCode(const QString &sSecrteCode)
{
    m_sSecrteCode = sSecrteCode;
}

QString CApp::getSSecretVector() const
{
    return m_sSecretVector;
}

void CApp::setSSecretVector(const QString &sSecretVector)
{
    m_sSecretVector = sSecretVector;
}

QString CApp::getSBaseUrl() const
{
    return m_sBaseUrl;
}

void CApp::setSBaseUrl(const QString &sBaseUrl)
{
    m_sBaseUrl = sBaseUrl;
}

QString CApp::getSMqUrl() const
{
    return m_sMqUrl;
}

void CApp::setSMqUrl(const QString &sMqUrl)
{
    m_sMqUrl = sMqUrl;
}

QString CApp::getSMqPort() const
{
    return m_sMqPort;
}

void CApp::setSMqPort(const QString &sMqPort)
{
    m_sMqPort = sMqPort;
}

QString CApp::getSMqUser() const
{
    return m_sMqUser;
}

void CApp::setSMqUser(const QString &sMqUser)
{
    m_sMqUser = sMqUser;
}

QString CApp::getSMqPassword() const
{
    return m_sMqPassword;
}

void CApp::setSMqPassword(const QString &sMqPassword)
{
    m_sMqPassword = sMqPassword;
}

QString CApp::getSRemoteIP() const
{
    return m_sRemoteIP;
}

void CApp::setSRemoteIP(const QString &sRemoteIP)
{
    m_sRemoteIP = sRemoteIP;
}

int CApp::getDRemotePort() const
{
    return m_dRemotePort;
}

void CApp::setDRemotePort(int dRemotePort)
{
    m_dRemotePort = dRemotePort;
}

int CApp::getDDeltaTime() const
{
    return m_dDeltaTime;
}

void CApp::setDDeltaTime(int dDeltaTime)
{
    m_dDeltaTime = dDeltaTime;
}

QString CApp::getSGatewaySn() const
{
    return m_sGatewaySn;
}

void CApp::setSGatewaySn(const QString &sGatewaySn)
{
    m_sGatewaySn = sGatewaySn;
}

QByteArray CApp::getBSecretCode() const
{
    return m_bSecretCode;
}

QByteArray CApp::getBSecretVector() const
{
    return m_bSecretVector;
}

std::string CApp::getKey() const
{
    return m_key;
}

std::string CApp::getIv() const
{
    return m_iv;
}

float CApp::getSharpRate() const
{
    return m_sharpRate;
}

void CApp::setSharpRate(float sharpRate)
{
    m_sharpRate = sharpRate;
}

float CApp::getSharpService() const
{
    return m_sharpService;
}

void CApp::setSharpService(float sharpService)
{
    m_sharpService = sharpService;
}

float CApp::getPeakRate() const
{
    return m_peakRate;
}

void CApp::setPeakRate(float peakRate)
{
    m_peakRate = peakRate;
}

float CApp::getPeakService() const
{
    return m_peakService;
}

void CApp::setPeakService(float peakService)
{
    m_peakService = peakService;
}

float CApp::getFlatRate() const
{
    return m_flatRate;
}

void CApp::setFlatRate(float flatRate)
{
    m_flatRate = flatRate;
}

float CApp::getFlatService() const
{
    return m_flatService;
}

void CApp::setFlatService(float flatService)
{
    m_flatService = flatService;
}

float CApp::getValleyRate() const
{
    return m_valleyRate;
}

void CApp::setValleyRate(float valleyRate)
{
    m_valleyRate = valleyRate;
}

float CApp::getValleyService() const
{
    return m_valleyService;
}

void CApp::setValleyService(float valleyService)
{
    m_valleyService = valleyService;
}

float CApp::getSharpTotal()
{
    return m_sharpRate + m_sharpService;
}

float CApp::getpeakTotal()
{
    return m_peakRate + m_peakService;
}

float CApp::getFlatTotal()
{
    return m_flatRate + m_flatService;
}

float CApp::getValleyTotal()
{
    return m_valleyRate + m_valleyService;
}

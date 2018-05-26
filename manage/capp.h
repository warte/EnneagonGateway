#ifndef CAPP_H
#define CAPP_H

/*
 * 网关属性
 * 充电桩属性
 * 后台属性
*/
#include <QObject>
#include <string>
#include <QSqlRecord>
#include "utility/cdatahelper.h"

class CApp : public QObject
{
    Q_OBJECT
public:
    explicit CApp(QObject *parent = 0);

    void        init();

    bool        isMqStandSame(QString url, QString port, QString user, QString pwd);

    void        getSysSetting(QSqlRecord rec);

    void        getRateSetting(QSqlRecord rec);

public:
    static CApp*   mInstance;
    static CApp*   getInstance();

    QString getSStationID() const;
    void setSStationID(const QString &sStationID);

    QString getSEnterpriseCode() const;
    void setSEnterpriseCode(const QString &sEnterpriseCode);

    QString getSSecrteCode() const;
    void setSSecrteCode(const QString &sSecrteCode);

    QString getSSecretVector() const;
    void setSSecretVector(const QString &sSecretVector);

    QString getSBaseUrl() const;
    void setSBaseUrl(const QString &sBaseUrl);

    QString getSMqUrl() const;
    void setSMqUrl(const QString &sMqUrl);

    QString getSMqPort() const;
    void setSMqPort(const QString &sMqPort);

    QString getSMqUser() const;
    void setSMqUser(const QString &sMqUser);

    QString getSMqPassword() const;
    void setSMqPassword(const QString &sMqPassword);

    QString getSRemoteIP() const;
    void setSRemoteIP(const QString &sRemoteIP);

    int getDRemotePort() const;
    void setDRemotePort(int dRemotePort);

    int getDDeltaTime() const;
    void setDDeltaTime(int dDeltaTime);

    QString getSGatewaySn() const;
    void setSGatewaySn(const QString &sGatewaySn);

    QByteArray getBSecretCode() const;

    QByteArray getBSecretVector() const;

    std::string getKey() const;

    std::string getIv() const;

    float   getSharpTotal();
    float   getpeakTotal();
    float   getFlatTotal();
    float   getValleyTotal();

    float getSharpRate() const;
    void setSharpRate(float sharpRate);

    float getSharpService() const;
    void setSharpService(float sharpService);

    float getPeakRate() const;
    void setPeakRate(float peakRate);

    float getPeakService() const;
    void setPeakService(float peakService);

    float getFlatRate() const;
    void setFlatRate(float flatRate);

    float getFlatService() const;
    void setFlatService(float flatService);

    float getValleyRate() const;
    void setValleyRate(float valleyRate);

    float getValleyService() const;
    void setValleyService(float valleyService);

private:
    QString     m_sGatewaySn;
    QString     m_sStationID;
    QString     m_sEnterpriseCode;
    QString     m_sSecrteCode;
    QString     m_sSecretVector;

    QByteArray  m_bSecretCode;
    QByteArray  m_bSecretVector;

    std::string m_key;
    std::string m_iv;

    QString     m_sBaseUrl;
    QString     m_sMqUrl;
    QString     m_sMqPort;
    QString     m_sMqUser;
    QString     m_sMqPassword;
    QString     m_sRemoteIP;
    int         m_dRemotePort;
    int         m_dDeltaTime;

    float       m_sharpRate;
    float       m_sharpService;
    float       m_peakRate;
    float       m_peakService;
    float       m_flatRate;
    float       m_flatService;
    float       m_valleyRate;
    float       m_valleyService;

signals:

public slots:
};

#endif // CAPP_H

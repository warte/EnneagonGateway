#ifndef CSTATEHTTP_H
#define CSTATEHTTP_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include <QDebug>
#include "utility/json.h"
#include "utility/aeshandler.h"

class CStateHttp : public QObject
{
    Q_OBJECT
public:
    explicit CStateHttp(QObject *parent = 0);

    void    init();

    void    request(const QByteArray& data);

private:
    QNetworkAccessManager*  m_manager;

    QNetworkReply*          m_reply;

    QTimer*                 m_timer;

    bool                    m_bRet;

    bool                    m_bRequesting;

    QString                 httpUrl;

signals:
    /*
     *
     */
    void    sendRetMsg(const QByteArray& data);

public slots:
    void    reqReply();

    void    reqTimeout();
};

#endif // CSTATEHTTP_H

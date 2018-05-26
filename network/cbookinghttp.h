#ifndef CBOOKINGHTTP_H
#define CBOOKINGHTTP_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include <QDebug>
#include "utility/cconfigdatahelper.h"
#include "utility/json.h"

class CBookingHttp : public QObject
{
    Q_OBJECT
public:
    explicit CBookingHttp(QObject *parent = 0);

    void    init();

    void    request(const QByteArray& data);

private:
    QNetworkAccessManager*  m_manager;

    QNetworkReply*          m_reply;

    QTimer*                 m_timer;

    QTimer*                 m_pushTimer;

    bool                    m_bRet;

    bool                    m_bRequesting;

    QString                 httpUrl;

signals:
    /*
         *
        */
    void    sendRetMsg(const QByteArray& data);

public slots:
    /*
             *
            */
    void    reqReply();

    /*
                 *
                */
    void    reqTimeout();

    void    pushTimeout();
};

#endif // CBOOKINGHTTP_H

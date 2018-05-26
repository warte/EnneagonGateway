#ifndef CREGHTTP_H
#define CREGHTTP_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include <QDebug>
#include "utility/json.h"
#include "utility/aeshandler.h"

class CRegHttp : public QObject
{
    Q_OBJECT
public:
    explicit CRegHttp(QObject *parent = 0);

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
    /*
             *
            */
    void    reqReply();

    /*
                 *
                */
    void    reqTimeout();
};

#endif // CREGHTTP_H

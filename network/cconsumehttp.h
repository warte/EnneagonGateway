#ifndef CCONSUMEHTTP_H
#define CCONSUMEHTTP_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>
#include <QDebug>
#include <QList>
#include <QMap>
#include <unistd.h>
#include <QUrl>
#include "utility/json.h"

class CConsumeHttp : public QObject
{
    Q_OBJECT
public:
    CConsumeHttp(QObject *parent = 0);

    void    init();

    void    request(const QString &connector, const QString& flowNo, const QByteArray& data);

private:
    QNetworkAccessManager*  m_manager;

    QNetworkReply*          m_reply;

    QTimer*                 m_timer;

    /*
     * 链表中的数据需要全部发送。定时10s检查一次。
     * 服务器有返回确认收到信息，才终止发送，并且将链表中的数据删除
    */
    QByteArray              m_consumeData;

    QString                 flow;

    QString                 connecorID;

    bool                    m_bRet;

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

#endif // CCONSUMEHTTP_H

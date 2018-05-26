#ifndef CREMOTESOCKET_H
#define CREMOTESOCKET_H

#include <QObject>
#include <QTcpSocket>
#include <QTimer>
#include <QHostAddress>
#include <QSettings>
#include "manage/capp.h"
#include "utility/json.h"

class CRemoteSocket : public QObject
{
    Q_OBJECT
public:
    explicit CRemoteSocket(QObject *parent = 0);

    void    init();

    void    publishTcpMsg(const QString& type, const QByteArray& data);

private:
    QTcpSocket  *m_sock;
    /*
     *
    */
    bool    m_bConnected;

    int     timeoutCounts;

    bool    m_bOffline;

    /*
     * 连接定时器，定时查看连接是否正常，掉线立刻重连,没有连接上立刻重连
    */
    QTimer  *m_connectTimer;

signals:
    //bool    backgroundOnLine(bool bOnline);

public slots:
    /*
     *  发送报文
    */
    void    sendMsgToServer(const QByteArray& data);
    /*
     * 检查连接情况
    */
    void    checkConnect();
    /*
     * 掉线重连
    */
    void    connnectToHost();
    /*
     * 已经连接上服务器
    */
    void    connectedToHost();

    void    readServerMsg();

    void    getError(QAbstractSocket::SocketError code);
};

#endif // CREMOTESOCKET_H

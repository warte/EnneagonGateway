#include "cremotesocket.h"
#include <QDateTime>
#include "manage/cmanage.h"

CRemoteSocket::CRemoteSocket(QObject *parent) : QObject(parent)
{

}

void CRemoteSocket::init()
{
    m_sock = new QTcpSocket(this);

    m_bConnected = false;

    m_bOffline = false;

    timeoutCounts = 0;

    connect(m_sock, SIGNAL(disconnected()), this, SLOT(connnectToHost()));
    connect(m_sock, SIGNAL(connected()), this, SLOT(connectedToHost()));
    connect(m_sock,SIGNAL(readyRead()),this,SLOT(readServerMsg()));
    connect(m_sock, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(getError(QAbstractSocket::SocketError)));

    QHostAddress add(CApp::getInstance()->getSRemoteIP());
    m_sock->connectToHost(add, CApp::getInstance()->getDRemotePort());

    m_connectTimer = new QTimer(this);
    connect(m_connectTimer, SIGNAL(timeout()), this, SLOT(checkConnect()));
    m_connectTimer->start(10000);
}

void CRemoteSocket::getError(QAbstractSocket::SocketError code)
{
    qDebug() << "remote error code" << code;
    m_bConnected = false;
    //emit backgroundOnLine(false);
}

void CRemoteSocket::connnectToHost()
{
    m_bConnected = false;

    QHostAddress add(CApp::getInstance()->getSRemoteIP());
    m_sock->connectToHost(add, CApp::getInstance()->getDRemotePort());
}

void CRemoteSocket::connectedToHost()
{
    m_bConnected = true;
    m_bOffline = false;

    qDebug() << "enneagon connected...";
}

/*
 * 如果连接不成功，每隔5秒重连一次
*/
void CRemoteSocket::checkConnect()
{
    qDebug() << "check enneagon connect..." << m_bConnected;
    timeoutCounts += 1;
    if(!m_bConnected)
    {
        QHostAddress add(CApp::getInstance()->getSRemoteIP());
        m_sock->connectToHost(add, CApp::getInstance()->getDRemotePort());

        //emit backgroundOnLine(false);
    }
}

void CRemoteSocket::readServerMsg()
{
    QByteArray m_data = m_sock->readAll();
}

void CRemoteSocket::publishTcpMsg(const QString &type, const QByteArray &data)
{
    if(m_bConnected)
    {
        qDebug() << QDateTime::currentDateTime().toString("yyyy-Mm-dd hh:mm:ss") << "send MSG to remote:" << data;
        m_sock->write(data);
    }
}

/*
 * 发送数据
*/
void    CRemoteSocket::sendMsgToServer(const QByteArray& data)
{
    if(m_bConnected)
    {
        qDebug() << QDateTime::currentDateTime().toString("yyyy-Mm-dd hh:mm:ss") << "send MSG to remote:" << data;
        m_sock->write(data);
    }
}

#ifndef CANWORKERTHREAD_H
#define CANWORKERTHREAD_H

#include <QThread>
#include <QDebug>
#include <QProcess>
#include <QFile>
#include <QTextStream>
#include <QByteArray>
#include <QDateTime>
#include "canwrapper.h"

class CanWorkerThread : public QThread
{
    Q_OBJECT
public:
    explicit CanWorkerThread(QObject *parent = 0);

    void    Init(CanWrapper *wrapper); // init

    void    run();         // start thread

    void    shutDown();    // Make thread shut down

    void    saveErrorCanMsg(QByteArray data);

signals:
    void    resetDevice();

    void    sendCanMsg(QByteArray data);

    void    sendSavedCanMsg(QByteArray data);

public slots:
    void    sendReplyData(QByteArray data);

private:
    CanWrapper *m_can;  // decode can

    bool    m_bGetRecv;

    int     m_dTimes;

    bool    m_running;
};

#endif // CANWORKERTHREAD_H

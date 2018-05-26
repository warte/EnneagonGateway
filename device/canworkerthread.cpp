#include "canworkerthread.h"

#include "canwrapper.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/uio.h>
#include <net/if.h>
#include <linux/can.h>
#include <linux/can/raw.h>

#include <signal.h>

#include <assert.h>

#include <errno.h>

#include <fcntl.h>

#include <stdio.h>

#include <sys/time.h>

CanWorkerThread::CanWorkerThread(QObject *parent) :
    QThread(parent)
{
}

// init worker thread
void CanWorkerThread::Init(CanWrapper *wrapper)
{
    m_running = true;
    m_bGetRecv = false;
    m_can = wrapper;
    m_dTimes = 0;   
}

void CanWorkerThread::run()
{
    struct can_frame msg;
    int ret;
    int i;
    int errorCode;

    bool rtr;
    bool extended;
    bool error;

    struct timeval tv;

    tv.tv_sec = 1;
    tv.tv_usec = 0;

    while(m_running)
    {
        ret = m_can->GetMsg(msg, extended, rtr, error, errorCode, tv);

        if(ret)
        {
            QString string;

            if(error)
            {
                string = QString("Error frame received, class = " +  QString::number(msg.can_id));
            }
            else if(extended)   // extent frame
            {
                if(rtr)
                {
                    string = QString("RTR ID: %1 LENGTH: %2").arg(msg.can_id).arg(msg.can_dlc);
                }
                else
                {
                    QByteArray da;
                    da.resize(12);
                    da[0] = (uchar)((0xff000000 & msg.can_id) >> 24);
                    da[1] = (uchar)((0x00ff0000 & msg.can_id) >> 16);
                    da[2] = (uchar)((0x0000ff00 & msg.can_id) >> 8);
                    da[3] = (uchar) (0x000000ff & msg.can_id);

                    m_bGetRecv = true;
                    m_dTimes = 0;

                    if((da[0] & 0xff) == 0x18)
                    {
                        // 保存，发送
                        if(((da[1] & 0xff) == 0x3a) | ((da[1] & 0xff) == 0x3b) | ((da[1] & 0xff) == 0x3c))
                        {
                            saveErrorCanMsg(da);
                        }
                    }

                    // can报文
                    if((msg.can_dlc == 8))
                    {
                        string = QString::number(msg.can_id, 16) + " ";
                        for(i = 0; i < msg.can_dlc; i++)
                        {
                            string.append(QString::number(msg.data[i], 16) + " ");
                        }

                        qDebug() << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << " can recv msg..." << string;

                        for(int i  = 0; i < 8; i++)
                        {
                            da[4+i] = msg.data[i];
                        }

                        emit sendCanMsg(da);
                    }
                }
            }
        }
    }
}

void CanWorkerThread::shutDown()
{
    m_running = false;
}

void CanWorkerThread::saveErrorCanMsg(QByteArray data)
{
    qDebug() << "set error";
    QFile file("pileerr.txt");
    if(!file.open(QIODevice::ReadWrite | QIODevice::Append | QIODevice::Text))
    {
        qDebug() << "can not open pileerr.txt file...";
        return;
    }

    QTextStream out(&file);
    QString str;
    for(int i = 0; i < 4; i++)
    {
        str += QString::number(data[i], 16);
    }

    str += " ";

    for(int i = 0; i < 8; i++)
    {
        str += QString::number(data[4+i], 16) + " ";
    }


    QDateTime curTime = QDateTime::currentDateTime();

    out  << curTime.toString("yyyy-MM-dd hh:mm:ss") << "    " << str;

    file.close();
}

void CanWorkerThread::sendReplyData(QByteArray data)
{
    QString str;

    int errorcode = 0;
    bool bOk = false;
    struct can_frame msg;
    msg.can_id = (int)(((data[0] & 0xff) << 24) | ((data[1] & 0xff) << 16) | ((data[2] & 0xff) << 8) | (data[3] & 0xff));
    str = QString::number(msg.can_id, 16) + "   ";
    msg.can_dlc = 8;
    for(int i = 0; i < 8; i++)
    {
        msg.data[i] = data[4 + i];

        str.append(QString::number(data[4 + i], 16) + " ");
    }

    qDebug() << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss") << "can send msg..." << str;

    m_can->SendMsg(msg, true, false, errorcode);
    if(errorcode != 0)
    {
        qDebug() << "can send error code" << errorcode;

        emit resetDevice();

        usleep(100 * 1000);
    }

}

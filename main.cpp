#include <QCoreApplication>
#include <QDebug>
#include <QTextCodec>
#include "manage/cmanage.h"

void customMessageHandler(QtMsgType type, const char *msg)
{
    QString txt;
    switch (type) {
    //调试信息提示
    case QtDebugMsg:
        txt = QString("Debug: %1").arg(msg);
        break;
        //一般的warning提示
    case QtWarningMsg:
        txt = QString("Warning: %1").arg(msg);
        break;
        //严重错误提示
    case QtCriticalMsg:
        txt = QString("Critical: %1").arg(msg);
        break;
        //致命错误提示
    case QtFatalMsg:
        txt = QString("Fatal: %1").arg(msg);
        abort();
    }

    QFile outFile("debuglog.txt");
    if(outFile.size() >  5 * 1024 * 1024)
    {
        outFile.open(QIODevice::WriteOnly);
    }
    else
    {
        outFile.open(QIODevice::WriteOnly | QIODevice::Append);
    }

    QTextStream ts(&outFile);
    ts << txt << endl;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qInstallMsgHandler(customMessageHandler);

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("GB2312"));

    CManage* manager = CManage::getInstance();


    return a.exec();
}

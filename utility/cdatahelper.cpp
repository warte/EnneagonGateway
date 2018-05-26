#include "cdatahelper.h"
#include "manage/capp.h"
#include "pile/cconnectormanager.h"
#include "utility/json.h"
#include "cdatauploader.h"


CPileDataHelper* CPileDataHelper::mInstance = NULL;

CPileDataHelper::CPileDataHelper(QObject *parent) : QObject(parent)
{

}

CPileDataHelper::~CPileDataHelper()
{
    _db.close();
}

// 打开本地数据库
CPileDataHelper* CPileDataHelper::getInstance()
{
    if(!mInstance)
    {
        mInstance = new CPileDataHelper();
        bool bRet = mInstance->openDatabase();

        qDebug() << "open database ..." << bRet;
    }

    return mInstance;
}

QString CPileDataHelper::getName()
{
    return _db.databaseName();
}

void CPileDataHelper::setDatabaseName(QString table)
{
    _tableName = table + ".db";
}

bool CPileDataHelper::openDatabase()
{
    _db = QSqlDatabase::addDatabase("QSQLITE");
    _db.setDatabaseName("/opt/enneagon/enneagon.db");

    if(!_db.open())
    {
        qDebug() << "can not open database";
        return false;
    }
    else
    {
        return true;
    }
}

void CPileDataHelper::getValues(const QString &expression, QStringList &names, QList<QStringList> &values)
{
    QSqlQuery query(QSqlDatabase::database());
    query.exec(expression);
    while(query.next())
    {
        QStringList res;
        for(int i = 0; i < names.size(); i++)
        {
            res << query.value(i).toString();
        }

        values.push_back(res);
    }
}

void CPileDataHelper::getValue(const QString &expression, QString &name, QStringList &values)
{
    QSqlQuery query(QSqlDatabase::database());
    query.exec(expression);
    while(query.next())
    {
        values << query.value(0).toString();
    }
}

void CPileDataHelper::queryOrderList()
{
    // 获取所有尚未结算的充电数据
    QSqlQuery query(QSqlDatabase::database());

    QString sql = "select * from consume where send_success='0'";

    query.exec(sql);
    if(query.next())
    {
        QSqlRecord record = query.record();
        CDataUploader::getInstance()->getDataForUploaded(record);

        //COrderManager::getInstance()->queryUnPublishOrder(record);
    }
}

void CPileDataHelper::activatePile(QString sn, bool bret, QString enCode, int enID, QString sitecode)
{
    QSqlQuery query(_db);
    int dreg;
    if(bret)
    {
        dreg = 1;
    }
    else
    {
        dreg = 0;
    }

    QString sql = "update dev set isReg='" + QString::number(dreg) + "',enterpriseCode='" + enCode +
            "',enterpriseID='" + QString::number(enID) + "',stationID='" + (sitecode) + "' where chargerSn='" + sn + "'";

    query.exec(sql);


    sql = "update info set enterpriseCode='" + enCode + "',enterpriseID='" + QString::number(enID)
            + "',stationID='"  + sitecode + "'";

    query.exec(sql);
}

void CPileDataHelper::queryPile()
{
    QSqlQuery query(_db);

    QString sql = "select * from dev";
    query.exec(sql);

    while(query.next())
    {
        QSqlRecord rec = query.record();

        CConnectorManager::getInstance()->getSysPileMsg(rec);
    }
}

void CPileDataHelper::queryRateSetting()
{
    QSqlQuery query(_db);
    QString sql = "select * from rate";
    query.exec(sql);
    while(query.next())
    {
        QSqlRecord rec = query.record();
        CApp::getInstance()->getRateSetting(rec);
    }
}

void CPileDataHelper::queryWarning(QString code)
{
    QSqlQuery query(_db);
    QString sql = "select * from alarm_item where code='" + code +"'";
    query.exec(sql);
    while (query.next()) {
        QSqlRecord rec = query.record();
        CConnectorManager::getInstance()->getSysWarning(rec);
    }
}

void CPileDataHelper::addWarningMsg(QString connectorCode, QString alarmCode, QString startTime, QString stopTime)
{
    QSqlQuery query(_db);
    QString sql = "insert into alarm_record(alarm_code,startTime,stopTime,connectorID) Values('" + alarmCode + "','" +
            startTime + "','" + stopTime + "','" + connectorCode + "')";
    if(!query.exec(sql))
    {
        qDebug() << query.lastError();
    }
}

void CPileDataHelper::updateMqInfo(QString host, QString port, QString user, QString pwd)
{
    QSqlQuery query(QSqlDatabase::database());
    QString sql = "update info set mqUrl='" + host + "',mqPort='" + port + "',mqUser='" + user + "',mqPassword='" + pwd +"'";

    qDebug() << sql;

    if(!query.exec(sql))
    {
        qDebug() << query.lastError();
    }
}

void CPileDataHelper::completeOrder(const QString flow)
{
    QSqlQuery query(QSqlDatabase::database());
    QString sql = "update consume set send_success='1' where flowno='" + flow + "'";

    if(!query.exec(sql))
    {
        qDebug() << query.lastError();
    }
}

void CPileDataHelper::queryDefaultSetting()
{
    QSqlQuery query(_db);
    QString sql = "select * from info";

    query.exec(sql);
    if(query.next())
    {
        // 查询系统设置信息,后台信息，连接信息，企业编号等
        QSqlRecord rec = query.record();

        CApp::getInstance()->getSysSetting(rec);
    }

}

bool CPileDataHelper::isOrderExisted(QString connectorCode, QString pileflow)
{
    QSqlQuery query(_db);
    QString sql = "select * from consume where connectorcode='" + connectorCode + "' and connectorflowno='" + pileflow + "'";
    query.exec(sql);
    if(query.next())
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool CPileDataHelper::execSql(const QString &express)
{
    QSqlQuery query(QSqlDatabase::database());
    if(query.exec(express))
    {
        return true;
    }
    else
    {
        qDebug() << query.lastError();
        return false;
    }
}

bool CPileDataHelper::insert(QString &table, QStringList &names, QStringList &values)
{
    if(names.size() != values.size())
    {
        return false;
    }

    QSqlQuery query(QSqlDatabase::database());

    QString sql = QString("insert into ") + table + QString("(");
    int i;

    for(i = 0; i < names.size(); i++)
    {
        sql = sql + names.value(i);
        if(i != names.size() - 1)
        {
            sql += QString(",");
        }
        else
        {
            sql = sql + QString(")");
        }
    }

    sql = sql + QString(" Values(");

    for(i = 0; i < values.size(); i++)
    {
        sql = sql + QString("'") + values.value(i) + QString("'");
        if(i != values.size() - 1)
        {
            sql = sql + QString(",");
        }
    }

    sql = sql + QString(")");

    qDebug() << "sql..." << sql;

    if(query.exec(sql))
    {
        qDebug() << "exe sql .... success...";
        return true;
    }
    else
    {
        qDebug() << "exe sql .... failed...";
        qDebug() << query.lastError();
        return false;
    }
}


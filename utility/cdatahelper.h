#ifndef CDATAHELPER_H
#define CDATAHELPER_H

#include <QObject>
#include <QList>
#include <QMap>
#include <QDebug>
#include <QStringList>
#include <QtSql/QSqlTableModel>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlError>

class CPileDataHelper : public QObject
{
    Q_OBJECT
public:
    explicit CPileDataHelper(QObject *parent = 0);

    ~CPileDataHelper();

    void    setDatabaseName(QString table);

    bool    openDatabase();

    static  CPileDataHelper* mInstance;

    static  CPileDataHelper* getInstance();

    QString getName();

    void    queryOrderList();

    void    queryPile();

    void    queryDefaultSetting();

    /*
     *
    */
    void    queryWarning(QString code);

    void    queryRateSetting();

    void    queryRecordOrder();

    // 插入告警数据
    void    addWarningMsg(QString connectorCode, QString alarmCode, QString startTime, QString stopTime);

    void    updateMqInfo(QString host, QString port, QString user, QString pwd);

    bool    isOrderExisted(QString connectorCode, QString pileflow);

    void    activatePile(QString sn, bool bret, QString enCode, int enID, QString sitecode);

    void    completeOrder(const QString flowNo);

    void    getValues(const QString &expression, QStringList &names, QList<QStringList> &values);

    void    getValue(const QString &expression, QString &name, QStringList &values);

    bool    insert(QString &table, QStringList &names, QStringList &values);

    bool    execSql(const QString &express);


private:
    QSqlTableModel *model;
    QSqlDatabase    _db;
    QString         _tableName;

signals:

public slots:
};

#endif // CDATAHELPER_H

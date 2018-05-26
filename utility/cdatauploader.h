#ifndef CDATAUPLOADER_H
#define CDATAUPLOADER_H

#include <QObject>
#include <QTimer>
#include <QSqlRecord>
#include <QSqlQuery>
#include "cdatahelper.h"

class CDataUploader : public QObject
{
    Q_OBJECT
public:
    explicit CDataUploader(QObject *parent = 0);

    static  CDataUploader* mInstance;

    static  CDataUploader* getInstance();

    void    init();
    /*
     * 查找尚未上传的历史报文
    */
    void    searchForRecordForUploaded();

    void    getDataForUploaded(QSqlRecord rec);

private:
    QTimer  *mTimer;

signals:

public slots:
    void    checkTimeout();
};

#endif // CDATAUPLOADER_H

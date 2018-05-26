#include "crecordhttp.h"
#include "manage/capp.h"
#include "manage/cmanage.h"
#include "utility/cdatahelper.h"

CRecordHttp::CRecordHttp(QObject *parent) :
    QObject(parent)
{
    init();
}


void CRecordHttp::init()
{
    m_manager = new QNetworkAccessManager(this);
    m_bRet = false;
}

/*
 * 全部流水号都从新平台的地址上获取
*/
void CRecordHttp::request(const QString &connector, const QString& flowNo, const QByteArray& data)
{
    flow = flowNo;
    connecorID = connector;
    m_consumeData = data;

    qDebug() << "send record order" << connector << data;

    QUrl url;
    url.setUrl(CApp::getInstance()->getSBaseUrl() + "hisOrder");
    QNetworkRequest req;
    req.setUrl(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json;charset=UTF-8");
    m_reply =  m_manager->post(req,data);
    connect(m_reply, SIGNAL(finished()), this, SLOT(reqReply()));
    QTimer::singleShot(10000, this, SLOT(reqTimeout()));
}

void CRecordHttp::reqTimeout()
{
    if(m_bRet == true)
    {
        m_reply->close();
        m_bRet = false;
        deleteLater();
    }
    else
    {
        m_reply->close();
        /*
         * timeout can not sync with platform
        */

        deleteLater();
    }
}

// 接收后台返回数据
// 如果成功的话将数据更新到数据库，并且将该条记录删除
void CRecordHttp::reqReply()
{
    m_bRet = true;
    QByteArray tdata = m_reply->readAll();

    qDebug() << "record reply" << tdata;

    m_reply->close();

    if(tdata.size() == 0)
    {
        deleteLater();
        return;
    }

    AESHandler handler;

    int status;
    bool bok;

    QByteArray data = handler.decodeMsg(tdata, status);

    // get json
    QtJson::JsonObject dataRes = QtJson::parse(data, bok).toMap();

    QString code = dataRes["connectorCode"].toString();

    QString flow = dataRes["orderNo"].toString();

    // 订单管理
    //if(status == 200)
    {
        CPileDataHelper::getInstance()->completeOrder(flow);
    }

    deleteLater();
}

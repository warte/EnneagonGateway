#include "cconsumehttp.h"
#include "manage/capp.h"
#include "manage/cmanage.h"

CConsumeHttp::CConsumeHttp(QObject *parent) :
    QObject(parent)
{
    init();
}


void CConsumeHttp::init()
{
    m_manager = new QNetworkAccessManager(this);
    m_bRet = false;
}

/*
 * 全部流水号都从新平台的地址上获取
*/
void CConsumeHttp::request(const QString &connector, const QString& flowNo, const QByteArray& data)
{
    flow = flowNo;
    connecorID = connector;
    m_consumeData = data;

    QUrl url;
    url.setUrl(CApp::getInstance()->getSBaseUrl() + "updateOrder");
    QNetworkRequest req;
    req.setUrl(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json;charset=UTF-8");
    m_reply =  m_manager->post(req,data);
    connect(m_reply, SIGNAL(finished()), this, SLOT(reqReply()));
    QTimer::singleShot(10000, this, SLOT(reqTimeout()));
}

void CConsumeHttp::reqTimeout()
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
        //CManage::getInstance()->sendConsumeReqToNetwork(localFlow, m_consumeData);
        /*
         * timeout can not sync with platform
        */
        CConnectorManager::getInstance()->getPlatformConsumeRetMsg(connecorID, flow, false);

        deleteLater();
    }
}

// 接收后台返回数据
// 如果成功的话将数据更新到数据库，并且将该条记录删除
void CConsumeHttp::reqReply()
{
    m_bRet = true;
    QByteArray tdata = m_reply->readAll();
    m_reply->close();

    if(tdata.size() == 0)
    {
        CConnectorManager::getInstance()->getPlatformConsumeRetMsg(connecorID, flow, false);
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
    if(status == 200)
    {
        CConnectorManager::getInstance()->getPlatformConsumeRetMsg(connecorID, flow, true);
    }
    else
    {
        CConnectorManager::getInstance()->getPlatformConsumeRetMsg(connecorID, flow, false);
    }

    deleteLater();
}

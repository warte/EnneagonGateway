#include "cflownohttp.h"
#include "pile/cconnectormanager.h"
#include <unistd.h>
#include "manage/cmanage.h"

CFlowNoHttp::CFlowNoHttp(QObject *parent) : QObject(parent)
{
    init();
}

void CFlowNoHttp::init()
{
    m_manager = new QNetworkAccessManager(this);

    m_flowRet = false;
}

/*
 * 全部流水号都从新平台的地址上获取
 * connectorID: 桩号 + 枪号
 * data: 请求的数据，包括具体的桩号信息以及vin
*/
void CFlowNoHttp::request(const QString connectorID, const QString flow, const QByteArray& data)
{
    m_connectorID = connectorID;
    m_flowRet = false;

    QUrl url;
    url.setUrl(CApp::getInstance()->getSBaseUrl() + "createOrder");
    QNetworkRequest req;
    req.setUrl(url);

    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json; charset=UTF-8");
    m_reply = m_manager->post(req, data);
    connect(m_reply, SIGNAL(finished()), this, SLOT(reqReply()));
    QTimer::singleShot(5000, this, SLOT(reqTimeout()));
}

void CFlowNoHttp::reqTimeout()
{
    if(m_flowRet == false)
    {
        m_reply->close();

        CConnectorManager::getInstance()->getPlatformFlowRetMsg(m_connectorID, false);

        deleteLater();
    }
    else
    {
        deleteLater();

        m_flowRet = false;
    }
}

void CFlowNoHttp::reqReply()
{
    m_flowRet = true;

    QByteArray tdata = m_reply->readAll();
    m_reply->close();

    if(tdata.size() == 0)
    {
        CConnectorManager::getInstance()->getPlatformFlowRetMsg(m_connectorID, false);
        deleteLater();
        return;
    }

    AESHandler handler;

    int status;
    bool bok;

    QByteArray data = handler.decodeMsg(tdata, status);

    // get json
    QtJson::JsonObject dataRes = QtJson::parse(data, bok).toMap();

    QString connectorcode = dataRes["connectorCode"].toString();

    switch (status) {
    case 200:
        // 成功，创建订单
        CConnectorManager::getInstance()->getPlatformFlowRetMsg(connectorcode, true);
        break;
    case 500:
        // 创建流水号失败
        CConnectorManager::getInstance()->getPlatformFlowRetMsg(connectorcode, false);
        break;
    case 501:
        CConnectorManager::getInstance()->getPlatformFlowRetMsg(connectorcode, false);
        break;
    case 502:
        CConnectorManager::getInstance()->getPlatformFlowRetMsg(connectorcode, false);
        break;
    default:
        break;
    }

    deleteLater();
}
